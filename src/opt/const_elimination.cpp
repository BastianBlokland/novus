#include "internal/expr_matchers.hpp"
#include "internal/find_used_consts.hpp"
#include "internal/prog_rewrite.hpp"
#include "opt/opt.hpp"
#include "prog/expr/nodes.hpp"
#include "prog/expr/rewriter.hpp"
#include "prog/sym/const_id_hasher.hpp"
#include <algorithm>
#include <cassert>

namespace opt {

using ConstUsageMap =
    typename std::unordered_map<prog::sym::ConstId, unsigned int, prog::sym::ConstIdHasher>;

using ConstReplaceMap =
    typename std::unordered_map<prog::sym::ConstId, prog::expr::NodePtr, prog::sym::ConstIdHasher>;

class ConstEliminatorRewriter final : public prog::expr::Rewriter {
public:
  ConstEliminatorRewriter(
      const prog::Program& prog,
      prog::sym::FuncId funcId,
      prog::sym::ConstDeclTable* consts,
      ConstUsageMap usageMap) :
      m_prog{prog},
      m_funcId{funcId},
      m_consts{consts},
      m_constUsage{std::move(usageMap)},
      m_modified{false} {

    if (m_consts == nullptr) {
      throw std::invalid_argument{"Consts table cannot be null"};
    }
  }

  auto rewrite(const prog::expr::Node& expr) -> prog::expr::NodePtr override;

  auto hasModified() -> bool override { return m_modified; }

private:
  const prog::Program& m_prog;
  prog::sym::FuncId m_funcId;
  prog::sym::ConstDeclTable* m_consts;
  ConstUsageMap m_constUsage;
  ConstReplaceMap m_replaceMap;
  bool m_modified;

  [[nodiscard]] auto
  shouldBeEliminated(const prog::sym::ConstId id, const prog::expr::Node& assignExpr) -> bool;

  // Check if the given expression is very cheap (so we can afford removing the constant and just
  // calling the expression multiple times), or is VERY likely that it will be optimized out by
  // later optimization passes.
  auto isCheapIntrinsic(const prog::expr::Node& expr) -> bool;

  auto eliminate(const prog::sym::ConstId id, prog::expr::NodePtr assignExpr) -> void;
};

auto eliminateConsts(const prog::Program& prog) -> prog::Program {
  auto modified = false;
  return eliminateConsts(prog, modified);
}

auto eliminateConsts(const prog::Program& prog, bool& modified) -> prog::Program {
  return internal::rewrite(
      prog,
      [](const prog::Program& prog, prog::sym::FuncId funcId, prog::sym::ConstDeclTable* consts) {
        const auto& funcDef = prog.getFuncDef(funcId);

        // Create a map of how many times constants are used.
        auto constUsageMap  = ConstUsageMap{};
        auto findUsedConsts = internal::FindUsedConsts{&constUsageMap};
        funcDef.getBody().accept(&findUsedConsts);

        return std::make_unique<ConstEliminatorRewriter>(
            prog, funcId, consts, std::move(constUsageMap));
      },
      modified);
}

auto ConstEliminatorRewriter::rewrite(const prog::expr::Node& expr) -> prog::expr::NodePtr {
  /* Any constant declarations in a group (and not the last expr of the group) and whose constant
   * is used only once (or less) in further expressions is removed. Usage of the removed const
   * will be replaced by the assignment expression.
   *
   * Reason for only considering assignments that in a group (and not the last expression in a
   * group) is that we can be sure that its value is not used. Elimating const declarations in
   * other places is possible but will require special treatment and is not implemented yet.
   *
   * To avoid changing the behaviour of the program any constant declarations whose assignment
   * expression has side-effects (like calling an action or declaring another const) is left
   * alone.
   */

  switch (expr.getKind()) {
  case prog::expr::NodeKind::Group: {

    const auto* groupExpr = expr.downcast<prog::expr::GroupExprNode>();
    auto newNodes         = std::vector<prog::expr::NodePtr>{};
    newNodes.reserve(groupExpr->getChildCount());

    for (auto i = 0U; i != groupExpr->getChildCount(); ++i) {
      const auto& child = (*groupExpr)[i];
      auto isLastChild  = i == groupExpr->getChildCount() - 1;

      if (child.getKind() == prog::expr::NodeKind::Assign) {
        const auto* childAssignNode = child.downcast<prog::expr::AssignExprNode>();
        const auto constId          = childAssignNode->getConst();
        auto assignExpr             = rewrite((*childAssignNode)[0]);

        if (!isLastChild && shouldBeEliminated(constId, *assignExpr)) {
          eliminate(constId, std::move(assignExpr));
          continue;
        }

        // If the const was not eliminated then re-add the assignment to the group.
        newNodes.push_back(prog::expr::assignExprNode(*m_consts, constId, std::move(assignExpr)));
      } else {

        // Non assignments we rewrite and re-add to the group.
        newNodes.push_back(rewrite(child));
      }
    }

    // Return either a group (if we still have more then 1 expr) or just the single expression.
    if (newNodes.size() > 1) {
      return prog::expr::groupExprNode(std::move(newNodes));
    }
    return std::move(newNodes[0]);
  }
  case prog::expr::NodeKind::Const: {

    // If the constant is in the replace-map then we rewrite it to the assignment expression of
    // that constant.
    const auto* constExpr    = expr.downcast<prog::expr::ConstExprNode>();
    const auto replaceMapItr = m_replaceMap.find(constExpr->getId());
    if (replaceMapItr != m_replaceMap.end()) {
      // Note: we clone the expression again here, unfortunate but needed because we can be
      // replacing the same constant twice so both need ownership.
      return replaceMapItr->second->clone(nullptr);
    }
    [[fallthrough]];
  }
  default:
    return expr.clone(this);
  }
}

auto ConstEliminatorRewriter::shouldBeEliminated(
    const prog::sym::ConstId id, const prog::expr::Node& assignExpr) -> bool {

  // If the expression has side-effects then moving it around would change the program execution.
  if (internal::hasSideEffect(m_prog, assignExpr)) {
    return false;
  }

  // Check if the constant is used only once (or zero times), if so we can freely remove it.
  const auto usageItr = m_constUsage.find(id);
  if (usageItr == m_constUsage.end() || usageItr->second == 1) {
    return true;
  }

  // If its used more often then check if the expression is very cheap, if so we should eliminate
  // it anyway.
  return assignExpr.getKind() == prog::expr::NodeKind::Const || isCheapIntrinsic(assignExpr);
}

auto ConstEliminatorRewriter::isCheapIntrinsic(const prog::expr::Node& expr) -> bool {

  if (internal::isLiteral(expr)) {
    return true;
  }

  if (expr.getKind() == prog::expr::NodeKind::Call) {
    const auto* callExpr = expr.downcast<prog::expr::CallExprNode>();
    const auto& args     = callExpr->getArgs();
    const auto funcId    = callExpr->getFunc();
    const auto& funcDecl = m_prog.getFuncDecl(funcId);

    // For the intrinsic to be cheap all of its arguments also have to be cheap.
    if (!std::all_of(args.begin(), args.end(), [this](const prog::expr::NodePtr& n) {
          return isCheapIntrinsic(*n);
        })) {
      return false;
    }

    // Note: actions are never 'cheap' because they are impure so its actual cost is unknown, also
    // moving actions around could change the program behaviour.
    return !funcDecl.isAction() && funcDecl.getKind() != prog::sym::FuncKind::User;
  }

  return false;
}

auto ConstEliminatorRewriter::eliminate(const prog::sym::ConstId id, prog::expr::NodePtr assignExpr)
    -> void {

  // Remove the constId from the ConstDeclTable of the function.
  m_consts->erase(id);

  // Add the assignment expression to a map so we can replace the usage of this const by the
  // expression itself.
  assert(m_replaceMap.find(id) == m_replaceMap.end());
  m_replaceMap[id] = std::move(assignExpr);

  m_modified = true;
}

} // namespace opt
