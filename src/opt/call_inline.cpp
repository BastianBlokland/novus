#include "internal/const_remapper.hpp"
#include "internal/expr_matchers.hpp"
#include "internal/prog_rewrite.hpp"
#include "opt/opt.hpp"
#include "prog/expr/nodes.hpp"
#include "prog/expr/rewriter.hpp"
#include "prog/sym/const_id_hasher.hpp"
#include <sstream>
#include <unordered_set>

namespace opt {

class CallInlineRewriter final : public prog::expr::Rewriter {
public:
  CallInlineRewriter(
      const prog::Program& prog, prog::sym::FuncId funcId, prog::sym::ConstDeclTable* consts) :
      m_prog{prog}, m_funcId{funcId}, m_consts{consts}, m_modified{false} {

    if (m_consts == nullptr) {
      throw std::invalid_argument{"Consts table cannot be null"};
    }
  }

  auto isInlinable(const prog::expr::CallExprNode* callExpr) {
    const auto funcId = callExpr->getFunc();

    // Non-normal (fork or lazy) calls or calls to non-user funcs are not inlinable.
    if (callExpr->getMode() != prog::expr::CallMode::Normal ||
        !internal::isUserFunc(m_prog, funcId)) {
      return false;
    }

    // Recursive calls cannot be inlined.
    if (funcId == m_funcId) {
      return false;
    }

    // Recursive functions are not inlined.
    if (internal::isRecursive(m_prog, funcId)) {
      return false;
    }

    // TODO(bastian): Consider adding size constraints to the functions we inline.
    return true;
  }

  auto rewrite(const prog::expr::Node& expr) -> prog::expr::NodePtr override {
    if (expr.getKind() == prog::expr::NodeKind::Call) {
      auto* callExpr = expr.downcast<prog::expr::CallExprNode>();
      if (isInlinable(callExpr)) {
        m_modified = true;
        return inlineCall(callExpr);
      }
    }
    return expr.clone(this);
  }

  auto hasModified() -> bool override { return m_modified; }

  auto inlineCall(const prog::expr::CallExprNode* callExpr) -> prog::expr::NodePtr {
    const auto& tgtFuncId  = callExpr->getFunc();
    const auto& tgtFuncDef = m_prog.getFuncDef(tgtFuncId);
    const auto& tgtConsts  = tgtFuncDef.getConsts();

    // Register all the consts of the target function into this function.
    auto constMapping = registerTargetConsts(tgtConsts);

    auto tgtInputs = tgtConsts.getInputs();
    auto exprs     = std::vector<prog::expr::NodePtr>{};
    exprs.reserve(tgtInputs.size() + 1);

    // Create expressions to assign the constants that used to be inputs to the target function.
    for (auto i = 0U; i != tgtInputs.size(); ++i) {
      exprs.push_back(prog::expr::assignExprNode(
          *m_consts, constMapping.find(tgtInputs[i])->second, rewrite((*callExpr)[i])));
    }

    // Remap the constants to point to the newly registered constants.
    auto remapper     = internal::ConstRemapper{m_prog, *m_consts, constMapping};
    auto remappedExpr = remapper.rewrite(tgtFuncDef.getExpr());

    // Run another 'rewrite' pass on the remapped expression so we can also inline calls inside the
    // inlined function body.
    exprs.push_back(rewrite(*remappedExpr));

    return exprs.size() > 1 ? prog::expr::groupExprNode(std::move(exprs)) : std::move(exprs[0]);
  }

  auto registerTargetConsts(const prog::sym::ConstDeclTable& tgtConsts)
      -> internal::ConstRemapTable {

    auto table = internal::ConstRemapTable{};
    for (const auto& tgtConstId : tgtConsts.getAll()) {
      const auto& tgtConstDecl = tgtConsts[tgtConstId];

      std::ostringstream oss;
      oss << "__inlined_" << m_consts->getNextConstId().getNum() << '_' << tgtConstDecl.getName();

      auto remappedConstId = m_consts->registerLocal(oss.str(), tgtConstDecl.getType());

      table.insert({tgtConstId, remappedConstId});
    }
    return table;
  }

private:
  const prog::Program& m_prog;
  prog::sym::FuncId m_funcId;
  prog::sym::ConstDeclTable* m_consts;
  bool m_modified;
};

auto inlineCalls(const prog::Program& prog) -> prog::Program {
  return internal::rewrite(
      prog,
      [](const prog::Program& prog, prog::sym::FuncId funcId, prog::sym::ConstDeclTable* consts) {
        return std::make_unique<CallInlineRewriter>(prog, funcId, consts);
      });
}

} // namespace opt
