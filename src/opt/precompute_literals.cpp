#include "internal/expr_matchers.hpp"
#include "internal/intrinsics.hpp"
#include "internal/prog_rewrite.hpp"
#include "internal/utilities.hpp"
#include "opt/opt.hpp"
#include "prog/expr/node_call_dyn.hpp"
#include "prog/expr/nodes.hpp"
#include "prog/expr/rewriter.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace opt {

class PrecomputeRewriter final : public prog::expr::Rewriter {
public:
  PrecomputeRewriter(
      const prog::Program& prog, prog::sym::FuncId funcId, prog::sym::ConstDeclTable* consts) :
      m_prog{prog}, m_funcId{funcId}, m_consts{consts}, m_modified{false} {

    if (m_consts == nullptr) {
      throw std::invalid_argument{"Consts table cannot be null"};
    }
  }

  auto rewrite(const prog::expr::Node& expr) -> prog::expr::NodePtr override {
    switch (expr.getKind()) {
    case prog::expr::NodeKind::Call:
      return precomputeCall(*expr.downcast<prog::expr::CallExprNode>());
    case prog::expr::NodeKind::CallDyn:
      return precomputeCallDyn(*expr.downcast<prog::expr::CallDynExprNode>());
    case prog::expr::NodeKind::Switch:
      return precomputeSwitch(*expr.downcast<prog::expr::SwitchExprNode>());
    case prog::expr::NodeKind::Field:
      return precomputeField(*expr.downcast<prog::expr::FieldExprNode>());
    default:
      return expr.clone(this); // Unable to precompute, just make a clone.
    }
  }

  auto hasModified() -> bool override { return m_modified; }

private:
  const prog::Program& m_prog;
  prog::sym::FuncId m_funcId;
  prog::sym::ConstDeclTable* m_consts;
  bool m_modified;

  [[nodiscard]] auto precomputeCall(const prog::expr::CallExprNode& callExpr)
      -> prog::expr::NodePtr;

  [[nodiscard]] auto
  precomputeIntrinsicCall(const prog::expr::CallExprNode& callExpr, prog::sym::FuncKind funcKind)
      -> prog::expr::NodePtr;

  [[nodiscard]] auto precomputeReinterpretConvCall(const prog::expr::CallExprNode& callExpr)
      -> prog::expr::NodePtr;

  [[nodiscard]] auto precomputeCallDyn(const prog::expr::CallDynExprNode& callDynExpr)
      -> prog::expr::NodePtr;

  [[nodiscard]] auto precomputeSwitch(const prog::expr::SwitchExprNode& switchExpr)
      -> prog::expr::NodePtr;

  [[nodiscard]] auto precomputeField(const prog::expr::FieldExprNode& fieldExpr)
      -> prog::expr::NodePtr;
};

auto precomputeLiterals(const prog::Program& prog) -> prog::Program {
  auto modified = false;
  return precomputeLiterals(prog, modified);
}

auto precomputeLiterals(const prog::Program& prog, bool& modified) -> prog::Program {
  return internal::rewrite(
      prog,
      [](const prog::Program& prog, prog::sym::FuncId funcId, prog::sym::ConstDeclTable* consts) {
        return std::make_unique<PrecomputeRewriter>(prog, funcId, consts);
      },
      modified);
}

auto PrecomputeRewriter::precomputeCall(const prog::expr::CallExprNode& callExpr)
    -> prog::expr::NodePtr {

  const auto funcKind = m_prog.getFuncDecl(callExpr.getFunc()).getKind();

  if (internal::isPrecomputableIntrinsic(funcKind)) {
    // Precompute intrinsic call (like integer addition).
    return precomputeIntrinsicCall(callExpr, funcKind);
  } else if (funcKind == prog::sym::FuncKind::NoOp && callExpr.getChildCount() == 1) {
    // Precompute reinterpreting conversions (like enum to int).
    return precomputeReinterpretConvCall(callExpr);
  }

  // Unable to precompute this call, just make a clone.
  return callExpr.clone(this);
}

auto PrecomputeRewriter::precomputeReinterpretConvCall(const prog::expr::CallExprNode& callExpr)
    -> prog::expr::NodePtr {

  // Single argument 'NoOp' intrinsics are used as reinterpreting conversions, for supported
  // combinations we can precompute the conversion.

  auto newArg        = rewrite(callExpr[0]);
  const auto dstType = callExpr.getType();

  // For identity conversions we can just return the argument.
  if (newArg->getType() == dstType) {
    m_modified = true;
    return newArg;
  }

  if (internal::isLiteral(*newArg)) {
    // Try precomputing the conversion.
    auto precomputed = internal::maybePrecomputeReinterpretConv(m_prog, *newArg, dstType);
    if (precomputed != nullptr) {
      m_modified = true;
      return precomputed;
    }
  }

  // if we cannot precompute then construct a copy of the call.
  auto newArgs = std::vector<prog::expr::NodePtr>{};
  newArgs.push_back(std::move(newArg));
  return prog::expr::callExprNode(m_prog, callExpr.getFunc(), std::move(newArgs));
}

auto PrecomputeRewriter::precomputeIntrinsicCall(
    const prog::expr::CallExprNode& callExpr, prog::sym::FuncKind funcKind) -> prog::expr::NodePtr {
  /* Check if all the arguments are literals, to support precomputing nested calls we rewrite the
   * arguments first. */

  auto newArgs = internal::rewriteAll(callExpr.getArgs(), this);

  const auto allArgsAreLiterals =
      std::all_of(newArgs.begin(), newArgs.end(), [](const prog::expr::NodePtr& n) {
        return internal::isLiteral(*n);
      });

  if (allArgsAreLiterals) {
    if (auto result = internal::precomputeIntrinsic(m_prog, funcKind, newArgs)) {

      if (result->getType() == callExpr.getType()) {
        // If precomputed type matches the expected type we can simply use that.
        m_modified = true;
        return result;
      } else {
        // If precomputed type does not match try to precompute a conversion.
        if (auto precomputed =
                internal::maybePrecomputeReinterpretConv(m_prog, *result, callExpr.getType())) {
          m_modified = true;
          return precomputed;
        }
      }
    }
  }
  // if we cannot precompute then construct a copy of the call.
  return prog::expr::callExprNode(m_prog, callExpr.getFunc(), std::move(newArgs));
}

auto PrecomputeRewriter::precomputeCallDyn(const prog::expr::CallDynExprNode& callDynExpr)
    -> prog::expr::NodePtr {

  /* When a dynamic call is made to either a function literal or a closure directly we can
   * rewrite it to be just a static call. */

  const auto& tgtExpr = callDynExpr[0];

  if (tgtExpr.getKind() == prog::expr::NodeKind::LitFunc) {

    const auto* litFuncNode = tgtExpr.downcast<prog::expr::LitFuncNode>();
    const auto funcId       = litFuncNode->getFunc();
    auto newArgs            = internal::rewriteAll(callDynExpr.getArgs(), this);
    return prog::expr::callExprNode(
        m_prog, funcId, callDynExpr.getType(), std::move(newArgs), callDynExpr.getMode());

  } else if (tgtExpr.getKind() == prog::expr::NodeKind::Closure) {

    const auto* closureNode = tgtExpr.downcast<prog::expr::ClosureNode>();
    const auto funcId       = closureNode->getFunc();

    // Create the set of arguments for the static call, its a combination of both the dynamic
    // call arguments and the bound arguments.
    auto newArgs = internal::rewriteAll(callDynExpr.getArgs(), this);
    for (const auto& arg : closureNode->getBoundArgs()) {
      newArgs.push_back(rewrite(*arg));
    }

    return prog::expr::callExprNode(
        m_prog, funcId, callDynExpr.getType(), std::move(newArgs), callDynExpr.getMode());
  }

  // Unable to precompute this dynamic call, just make a clone.
  return callDynExpr.clone(this);
}

auto PrecomputeRewriter::precomputeSwitch(const prog::expr::SwitchExprNode& switchExpr)
    -> prog::expr::NodePtr {

  /* In switch statements the result can be precomputed if the conditions are literals. */

  auto newConditions = internal::rewriteAll(switchExpr.getConditions(), this);
  auto newBranches   = internal::rewriteAll(switchExpr.getBranches(), this);

  for (auto i = 0U; i != newConditions.size(); ++i) {
    if (newConditions[i]->getKind() == prog::expr::NodeKind::LitBool) {
      // If the condition is 'true' then the switch can be eliminated by just returning the
      // matching branch.
      if (internal::getBool(*newConditions[i])) {
        m_modified = true;
        return std::move(newBranches[i]);

      } else {
        // If the condition is 'false', then we can remove both the condition and the branch.
        newConditions.erase(newConditions.begin() + i);
        newBranches.erase(newBranches.begin() + i);
        --i;

        m_modified = true;
      }
    }
  }

  // If no conditions are left then we can eliminate the switch we can return the 'else' branch.
  if (newConditions.empty()) {
    assert(newBranches.size() == 1);
    return std::move(newBranches[0]);
  }
  return prog::expr::switchExprNode(m_prog, std::move(newConditions), std::move(newBranches));
}

auto PrecomputeRewriter::precomputeField(const prog::expr::FieldExprNode& fieldExpr)
    -> prog::expr::NodePtr {
  /* If we construct a struct and then immediately load a we can optimize away the struct
   * creation. */

  const auto fieldIndex = fieldExpr.getId().getNum();
  const auto& loadExpr  = fieldExpr[0];

  if (loadExpr.getKind() == prog::expr::NodeKind::Call) {
    const auto* loadCallExpr = loadExpr.downcast<prog::expr::CallExprNode>();
    const auto& loadFuncDecl = m_prog.getFuncDecl(loadCallExpr->getFunc());
    if (loadFuncDecl.getKind() == prog::sym::FuncKind::MakeStruct) {
      return rewrite(loadExpr[fieldIndex]);
    }
  }

  // Unable to precompute this field, just make a clone.
  return fieldExpr.clone(this);
}

} // namespace opt
