#include "internal/expr_matchers.hpp"
#include "internal/prog_rewrite.hpp"
#include "opt/opt.hpp"
#include "prog/expr/nodes.hpp"
#include "prog/expr/rewriter.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace opt {

[[nodiscard]] static auto getInt(const prog::expr::Node& n) -> int32_t {
  return n.downcast<prog::expr::LitIntNode>()->getVal();
}

[[nodiscard]] static auto getBool(const prog::expr::Node& n) -> bool {
  return n.downcast<prog::expr::LitBoolNode>()->getVal();
}

[[nodiscard]] static auto getFloat(const prog::expr::Node& n) -> float {
  return n.downcast<prog::expr::LitFloatNode>()->getVal();
}

[[nodiscard]] static auto getLong(const prog::expr::Node& n) -> int64_t {
  return n.downcast<prog::expr::LitLongNode>()->getVal();
}

[[nodiscard]] static auto getChar(const prog::expr::Node& n) -> uint8_t {
  return n.downcast<prog::expr::LitCharNode>()->getVal();
}

[[nodiscard]] static auto getString(const prog::expr::Node& n) -> std::string {
  return n.downcast<prog::expr::LitStringNode>()->getVal();
}

[[nodiscard]] static auto isSupportedIntrinsic(prog::sym::FuncKind funcKind) -> bool {
  switch (funcKind) {
  // Int
  case prog::sym::FuncKind::AddInt:
  case prog::sym::FuncKind::SubInt:
  case prog::sym::FuncKind::MulInt:
  case prog::sym::FuncKind::DivInt:
  case prog::sym::FuncKind::RemInt:
  case prog::sym::FuncKind::NegateInt:
  case prog::sym::FuncKind::IncrementInt:
  case prog::sym::FuncKind::DecrementInt:
  case prog::sym::FuncKind::ShiftLeftInt:
  case prog::sym::FuncKind::ShiftRightInt:
  case prog::sym::FuncKind::AndInt:
  case prog::sym::FuncKind::OrInt:
  case prog::sym::FuncKind::XorInt:
  case prog::sym::FuncKind::InvInt:
  case prog::sym::FuncKind::CheckEqInt:
  case prog::sym::FuncKind::CheckNEqInt:
  case prog::sym::FuncKind::CheckLeInt:
  case prog::sym::FuncKind::CheckLeEqInt:
  case prog::sym::FuncKind::CheckGtInt:
  case prog::sym::FuncKind::CheckGtEqInt:
  case prog::sym::FuncKind::ConvIntLong:
  case prog::sym::FuncKind::ConvIntFloat:
  case prog::sym::FuncKind::ConvIntString:
  case prog::sym::FuncKind::ConvIntChar:
  case prog::sym::FuncKind::DefInt:

  // Float
  case prog::sym::FuncKind::AddFloat:
  case prog::sym::FuncKind::SubFloat:
  case prog::sym::FuncKind::MulFloat:
  case prog::sym::FuncKind::DivFloat:
  case prog::sym::FuncKind::ModFloat:
  case prog::sym::FuncKind::PowFloat:
  case prog::sym::FuncKind::SqrtFloat:
  case prog::sym::FuncKind::SinFloat:
  case prog::sym::FuncKind::CosFloat:
  case prog::sym::FuncKind::TanFloat:
  case prog::sym::FuncKind::ASinFloat:
  case prog::sym::FuncKind::ACosFloat:
  case prog::sym::FuncKind::ATanFloat:
  case prog::sym::FuncKind::ATan2Float:
  case prog::sym::FuncKind::NegateFloat:
  case prog::sym::FuncKind::IncrementFloat:
  case prog::sym::FuncKind::DecrementFloat:
  case prog::sym::FuncKind::CheckEqFloat:
  case prog::sym::FuncKind::CheckNEqFloat:
  case prog::sym::FuncKind::CheckLeFloat:
  case prog::sym::FuncKind::CheckLeEqFloat:
  case prog::sym::FuncKind::CheckGtFloat:
  case prog::sym::FuncKind::CheckGtEqFloat:
  case prog::sym::FuncKind::ConvFloatInt:
  case prog::sym::FuncKind::ConvFloatString:
  case prog::sym::FuncKind::ConvFloatChar:
  case prog::sym::FuncKind::DefFloat:

  // Long
  case prog::sym::FuncKind::AddLong:
  case prog::sym::FuncKind::SubLong:
  case prog::sym::FuncKind::MulLong:
  case prog::sym::FuncKind::DivLong:
  case prog::sym::FuncKind::RemLong:
  case prog::sym::FuncKind::NegateLong:
  case prog::sym::FuncKind::IncrementLong:
  case prog::sym::FuncKind::DecrementLong:
  case prog::sym::FuncKind::CheckEqLong:
  case prog::sym::FuncKind::CheckNEqLong:
  case prog::sym::FuncKind::CheckLeLong:
  case prog::sym::FuncKind::CheckLeEqLong:
  case prog::sym::FuncKind::CheckGtLong:
  case prog::sym::FuncKind::CheckGtEqLong:
  case prog::sym::FuncKind::ConvLongInt:
  case prog::sym::FuncKind::ConvLongString:
  case prog::sym::FuncKind::DefLong:

  // Bool
  case prog::sym::FuncKind::InvBool:
  case prog::sym::FuncKind::CheckEqBool:
  case prog::sym::FuncKind::CheckNEqBool:
  case prog::sym::FuncKind::DefBool:

  // Char
  case prog::sym::FuncKind::CombineChar:
  case prog::sym::FuncKind::IncrementChar:
  case prog::sym::FuncKind::DecrementChar:
  case prog::sym::FuncKind::ConvCharString:

  // String
  case prog::sym::FuncKind::AddString:
  case prog::sym::FuncKind::LengthString:
  case prog::sym::FuncKind::IndexString:
  case prog::sym::FuncKind::SliceString:
  case prog::sym::FuncKind::AppendChar:
  case prog::sym::FuncKind::CheckEqString:
  case prog::sym::FuncKind::CheckNEqString:
  case prog::sym::FuncKind::DefString:

    return true;

  default:
    return false;
  }
}

[[nodiscard]] auto
rewriteAll(const std::vector<prog::expr::NodePtr>& nodes, prog::expr::Rewriter* rewriter)
    -> std::vector<prog::expr::NodePtr> {

  auto newNodes = std::vector<prog::expr::NodePtr>{};
  newNodes.reserve(nodes.size());

  for (auto itr = nodes.begin(); itr != nodes.end(); ++itr) {
    newNodes.push_back(rewriter->rewrite(**itr));
  }

  return newNodes;
}

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

    case prog::expr::NodeKind::Call: {
      /* Rewrite calls to intrinsic computations (for example integer negation) to which all
      arguments are literals. */

      const auto* callExpr = expr.downcast<prog::expr::CallExprNode>();
      const auto funcId    = callExpr->getFunc();
      const auto funcKind  = m_prog.getFuncDecl(funcId).getKind();

      // Check if the call is an intrinsic we support.
      if (isSupportedIntrinsic(funcKind)) {
        /* Now we need to check if all the arguments are literals, to support precomputing nested
         * calls we rewrite the arguments first. */

        auto newArgs = rewriteAll(callExpr->getArgs(), this);

        // If all the arguments are literals we can precompute the value.
        if (std::all_of(newArgs.begin(), newArgs.end(), [](const prog::expr::NodePtr& n) {
              return internal::isLiteral(*n);
            })) {
          m_modified = true;
          return precomputeIntrinsic(funcKind, std::move(newArgs));
        } else {
          // if we cannot precompute then construct a copy of the call.
          return prog::expr::callExprNode(m_prog, funcId, std::move(newArgs));
        }

      } else if (funcKind == prog::sym::FuncKind::NoOp && expr.getChildCount() == 1) {
        // Single argument 'NoOp' intrinsics are used as reinterpreting conversions, for supported
        // combinations we can precompute the conversion.

        auto newArg        = rewrite(expr[0]);
        const auto dstType = callExpr->getType();

        // For identity conversions we can just return the argument.
        if (newArg->getType() == dstType) {
          m_modified = true;
          return newArg;
        }

        if (internal::isLiteral(*newArg)) {
          // Try precomputing the conversion.
          auto precomputed = maybePrecomputeReinterpretConv(*newArg, dstType);
          if (precomputed != nullptr) {
            m_modified = true;
            return precomputed;
          }
        }

        // if we cannot precompute then construct a copy of the call.
        auto newArgs = std::vector<prog::expr::NodePtr>{};
        newArgs.push_back(std::move(newArg));
        return prog::expr::callExprNode(m_prog, funcId, std::move(newArgs));
      }
    } break;

    case prog::expr::NodeKind::CallDyn: {
      /* When a dynamic call is made to either a function literal or a closure directly we can
       * rewrite it to be just a static call. */

      const auto* dynCallExpr = expr.downcast<prog::expr::CallDynExprNode>();
      const auto& tgtExpr     = expr[0];

      if (tgtExpr.getKind() == prog::expr::NodeKind::LitFunc) {

        const auto* litFuncNode = tgtExpr.downcast<prog::expr::LitFuncNode>();
        const auto funcId       = litFuncNode->getFunc();
        auto newArgs            = rewriteAll(dynCallExpr->getArgs(), this);
        return prog::expr::callExprNode(
            m_prog, funcId, dynCallExpr->getType(), std::move(newArgs), dynCallExpr->isFork());

      } else if (tgtExpr.getKind() == prog::expr::NodeKind::Closure) {

        const auto* closureNode = tgtExpr.downcast<prog::expr::ClosureNode>();
        const auto funcId       = closureNode->getFunc();

        // Create the set of arguments for the static call, its a combination of both the dynamic
        // call arguments and the bound arguments.
        auto newArgs = rewriteAll(dynCallExpr->getArgs(), this);
        for (const auto& arg : closureNode->getBoundArgs()) {
          newArgs.push_back(rewrite(*arg));
        }

        return prog::expr::callExprNode(
            m_prog, funcId, dynCallExpr->getType(), std::move(newArgs), dynCallExpr->isFork());
      }

    } break;

    case prog::expr::NodeKind::Switch: {
      /* In switch statements the result can be precomputed if the conditions are literals. */

      const auto* switchExpr = expr.downcast<prog::expr::SwitchExprNode>();

      auto newConditions = rewriteAll(switchExpr->getConditions(), this);
      auto newBranches   = rewriteAll(switchExpr->getBranches(), this);
      return precomputeSwitch(std::move(newConditions), std::move(newBranches));

    } break;

    case prog::expr::NodeKind::Field: {
      /* If we construct a struct and then immediately load a we can optimize away the struct
       * creation. */

      const auto* fieldExpr = expr.downcast<prog::expr::FieldExprNode>();
      const auto fieldIndex = fieldExpr->getId().getNum();
      const auto& loadExpr  = expr[0];

      if (loadExpr.getKind() == prog::expr::NodeKind::Call) {
        const auto* loadCallExpr = loadExpr.downcast<prog::expr::CallExprNode>();
        const auto& loadFuncDecl = m_prog.getFuncDecl(loadCallExpr->getFunc());
        if (loadFuncDecl.getKind() == prog::sym::FuncKind::MakeStruct) {
          return rewrite(loadExpr[fieldIndex]);
        }
      }

    } break;

    default:
      break;
    }

    // If we don't want to rewrite this expression just make a clone.
    return expr.clone(this);
  }

  auto hasModified() -> bool override { return m_modified; }

private:
  const prog::Program& m_prog;
  prog::sym::FuncId m_funcId;
  prog::sym::ConstDeclTable* m_consts;
  bool m_modified;

  [[nodiscard]] auto precomputeSwitch(
      std::vector<prog::expr::NodePtr> conditions, std::vector<prog::expr::NodePtr> branches)
      -> prog::expr::NodePtr {

    for (auto i = 0U; i != conditions.size(); ++i) {
      if (conditions[i]->getKind() == prog::expr::NodeKind::LitBool) {
        // If the condition is 'true' then the switch can be eliminated by just returning the
        // matching branch.
        if (getBool(*conditions[i])) {
          m_modified = true;
          return std::move(branches[i]);

        } else {
          // If the condition is 'false', then we can remove both the condition and the branch.
          conditions.erase(conditions.begin() + i);
          branches.erase(branches.begin() + i);
          --i;

          m_modified = true;
        }
      }
    }

    // If no conditions are left then we can eliminate the switch we can return the 'else' branch.
    if (conditions.empty()) {
      assert(branches.size() == 1);
      return std::move(branches[0]);
    }
    return prog::expr::switchExprNode(m_prog, std::move(conditions), std::move(branches));
  }

  [[nodiscard]] auto
  precomputeIntrinsic(prog::sym::FuncKind funcKind, std::vector<prog::expr::NodePtr> args)
      -> prog::expr::NodePtr {

    switch (funcKind) {

    // Int
    case prog::sym::FuncKind::AddInt: {
      assert(args.size() == 2);
      return prog::expr::litIntNode(m_prog, getInt(*args[0]) + getInt(*args[1]));
    }
    case prog::sym::FuncKind::SubInt: {
      assert(args.size() == 2);
      return prog::expr::litIntNode(m_prog, getInt(*args[0]) - getInt(*args[1]));
    }
    case prog::sym::FuncKind::MulInt: {
      assert(args.size() == 2);
      return prog::expr::litIntNode(m_prog, getInt(*args[0]) * getInt(*args[1]));
    }
    case prog::sym::FuncKind::DivInt: {
      assert(args.size() == 2);
      return prog::expr::litIntNode(m_prog, getInt(*args[0]) / getInt(*args[1]));
    }
    case prog::sym::FuncKind::RemInt: {
      assert(args.size() == 2);
      return prog::expr::litIntNode(m_prog, getInt(*args[0]) % getInt(*args[1]));
    }
    case prog::sym::FuncKind::NegateInt: {
      assert(args.size() == 1);
      return prog::expr::litIntNode(m_prog, -getInt(*args[0]));
    }
    case prog::sym::FuncKind::IncrementInt: {
      assert(args.size() == 1);
      return prog::expr::litIntNode(m_prog, getInt(*args[0]) + 1);
    }
    case prog::sym::FuncKind::DecrementInt: {
      assert(args.size() == 1);
      return prog::expr::litIntNode(m_prog, getInt(*args[0]) - 1);
    }
    case prog::sym::FuncKind::ShiftLeftInt: {
      assert(args.size() == 2);
      return prog::expr::litIntNode(m_prog, getInt(*args[0]) << getInt(*args[1]));
    }
    case prog::sym::FuncKind::ShiftRightInt: {
      assert(args.size() == 2);
      return prog::expr::litIntNode(m_prog, getInt(*args[0]) >> getInt(*args[1]));
    }
    case prog::sym::FuncKind::AndInt: {
      assert(args.size() == 2);
      return prog::expr::litIntNode(m_prog, getInt(*args[0]) & getInt(*args[1]));
    }
    case prog::sym::FuncKind::OrInt: {
      assert(args.size() == 2);
      return prog::expr::litIntNode(m_prog, getInt(*args[0]) | getInt(*args[1]));
    }
    case prog::sym::FuncKind::XorInt: {
      assert(args.size() == 2);
      return prog::expr::litIntNode(m_prog, getInt(*args[0]) ^ getInt(*args[1]));
    }
    case prog::sym::FuncKind::InvInt: {
      assert(args.size() == 1);
      return prog::expr::litIntNode(m_prog, ~getInt(*args[0]));
    }
    case prog::sym::FuncKind::CheckEqInt: {
      assert(args.size() == 2);
      return prog::expr::litBoolNode(m_prog, getInt(*args[0]) == getInt(*args[1]));
    }
    case prog::sym::FuncKind::CheckNEqInt: {
      assert(args.size() == 2);
      return prog::expr::litBoolNode(m_prog, getInt(*args[0]) != getInt(*args[1]));
    }
    case prog::sym::FuncKind::CheckLeInt: {
      assert(args.size() == 2);
      return prog::expr::litBoolNode(m_prog, getInt(*args[0]) < getInt(*args[1]));
    }
    case prog::sym::FuncKind::CheckLeEqInt: {
      assert(args.size() == 2);
      return prog::expr::litBoolNode(m_prog, getInt(*args[0]) <= getInt(*args[1]));
    }
    case prog::sym::FuncKind::CheckGtInt: {
      assert(args.size() == 2);
      return prog::expr::litBoolNode(m_prog, getInt(*args[0]) > getInt(*args[1]));
    }
    case prog::sym::FuncKind::CheckGtEqInt: {
      assert(args.size() == 2);
      return prog::expr::litBoolNode(m_prog, getInt(*args[0]) >= getInt(*args[1]));
    }
    case prog::sym::FuncKind::ConvIntLong: {
      assert(args.size() == 1);
      return prog::expr::litLongNode(m_prog, static_cast<int64_t>(getInt(*args[0])));
    }
    case prog::sym::FuncKind::ConvIntFloat: {
      assert(args.size() == 1);
      return prog::expr::litFloatNode(m_prog, static_cast<float>(getInt(*args[0])));
    }
    case prog::sym::FuncKind::ConvIntString: {
      assert(args.size() == 1);
      auto ss = std::stringstream{};
      ss << getInt(*args[0]);
      return prog::expr::litStringNode(m_prog, ss.str());
    }
    case prog::sym::FuncKind::ConvIntChar: {
      assert(args.size() == 1);
      return prog::expr::litCharNode(m_prog, static_cast<uint8_t>(getInt(*args[0])));
    }
    case prog::sym::FuncKind::DefInt: {
      assert(args.size() == 0);
      return prog::expr::litIntNode(m_prog, 0);
    }

    // Float
    case prog::sym::FuncKind::AddFloat: {
      assert(args.size() == 2);
      return prog::expr::litFloatNode(m_prog, getFloat(*args[0]) + getFloat(*args[1]));
    }
    case prog::sym::FuncKind::SubFloat: {
      assert(args.size() == 2);
      return prog::expr::litFloatNode(m_prog, getFloat(*args[0]) - getFloat(*args[1]));
    }
    case prog::sym::FuncKind::MulFloat: {
      assert(args.size() == 2);
      return prog::expr::litFloatNode(m_prog, getFloat(*args[0]) * getFloat(*args[1]));
    }
    case prog::sym::FuncKind::DivFloat: {
      assert(args.size() == 2);
      return prog::expr::litFloatNode(m_prog, getFloat(*args[0]) / getFloat(*args[1]));
    }
    case prog::sym::FuncKind::ModFloat: {
      assert(args.size() == 2);
      return prog::expr::litFloatNode(m_prog, fmodf(getFloat(*args[0]), getFloat(*args[1])));
    }
    case prog::sym::FuncKind::PowFloat: {
      assert(args.size() == 2);
      return prog::expr::litFloatNode(m_prog, powf(getFloat(*args[0]), getFloat(*args[1])));
    }
    case prog::sym::FuncKind::SqrtFloat: {
      assert(args.size() == 1);
      return prog::expr::litFloatNode(m_prog, sqrtf(getFloat(*args[0])));
    }
    case prog::sym::FuncKind::SinFloat: {
      assert(args.size() == 1);
      return prog::expr::litFloatNode(m_prog, sinf(getFloat(*args[0])));
    }
    case prog::sym::FuncKind::CosFloat: {
      assert(args.size() == 1);
      return prog::expr::litFloatNode(m_prog, cosf(getFloat(*args[0])));
    }
    case prog::sym::FuncKind::TanFloat: {
      assert(args.size() == 1);
      return prog::expr::litFloatNode(m_prog, tanf(getFloat(*args[0])));
    }
    case prog::sym::FuncKind::ASinFloat: {
      assert(args.size() == 1);
      return prog::expr::litFloatNode(m_prog, asinf(getFloat(*args[0])));
    }
    case prog::sym::FuncKind::ACosFloat: {
      assert(args.size() == 1);
      return prog::expr::litFloatNode(m_prog, acosf(getFloat(*args[0])));
    }
    case prog::sym::FuncKind::ATanFloat: {
      assert(args.size() == 1);
      return prog::expr::litFloatNode(m_prog, atanf(getFloat(*args[0])));
    }
    case prog::sym::FuncKind::ATan2Float: {
      assert(args.size() == 2);
      return prog::expr::litFloatNode(m_prog, atan2f(getFloat(*args[0]), getFloat(*args[1])));
    }
    case prog::sym::FuncKind::NegateFloat: {
      assert(args.size() == 1);
      return prog::expr::litFloatNode(m_prog, -getFloat(*args[0]));
    }
    case prog::sym::FuncKind::IncrementFloat: {
      assert(args.size() == 1);
      return prog::expr::litFloatNode(m_prog, getFloat(*args[0]) + 1);
    }
    case prog::sym::FuncKind::DecrementFloat: {
      assert(args.size() == 1);
      return prog::expr::litFloatNode(m_prog, getFloat(*args[0]) - 1);
    }
    case prog::sym::FuncKind::CheckEqFloat: {
      assert(args.size() == 2);
      return prog::expr::litBoolNode(m_prog, getFloat(*args[0]) == getFloat(*args[1]));
    }
    case prog::sym::FuncKind::CheckNEqFloat: {
      assert(args.size() == 2);
      return prog::expr::litBoolNode(m_prog, getFloat(*args[0]) != getFloat(*args[1]));
    }
    case prog::sym::FuncKind::CheckLeFloat: {
      assert(args.size() == 2);
      return prog::expr::litBoolNode(m_prog, getFloat(*args[0]) < getFloat(*args[1]));
    }
    case prog::sym::FuncKind::CheckLeEqFloat: {
      assert(args.size() == 2);
      return prog::expr::litBoolNode(m_prog, getFloat(*args[0]) <= getFloat(*args[1]));
    }
    case prog::sym::FuncKind::CheckGtFloat: {
      assert(args.size() == 2);
      return prog::expr::litBoolNode(m_prog, getFloat(*args[0]) > getFloat(*args[1]));
    }
    case prog::sym::FuncKind::CheckGtEqFloat: {
      assert(args.size() == 2);
      return prog::expr::litBoolNode(m_prog, getFloat(*args[0]) >= getFloat(*args[1]));
    }
    case prog::sym::FuncKind::ConvFloatInt: {
      assert(args.size() == 1);
      return prog::expr::litIntNode(m_prog, static_cast<int32_t>(getFloat(*args[0])));
    }
    case prog::sym::FuncKind::ConvFloatString: {
      assert(args.size() == 1);
      auto f = getFloat(*args[0]);
      if (std::isnan(f)) {
        return prog::expr::litStringNode(m_prog, "nan");
      }
      auto ss = std::stringstream{};
      ss << std::setprecision(6) << f;
      return prog::expr::litStringNode(m_prog, ss.str());
    }
    case prog::sym::FuncKind::ConvFloatChar: {
      assert(args.size() == 1);
      return prog::expr::litCharNode(m_prog, static_cast<uint8_t>(getFloat(*args[0])));
    }
    case prog::sym::FuncKind::DefFloat: {
      assert(args.size() == 0);
      return prog::expr::litFloatNode(m_prog, 0.0);
    }

    // Long
    case prog::sym::FuncKind::AddLong: {
      assert(args.size() == 2);
      return prog::expr::litLongNode(m_prog, getLong(*args[0]) + getLong(*args[1]));
    }
    case prog::sym::FuncKind::SubLong: {
      assert(args.size() == 2);
      return prog::expr::litLongNode(m_prog, getLong(*args[0]) - getLong(*args[1]));
    }
    case prog::sym::FuncKind::MulLong: {
      assert(args.size() == 2);
      return prog::expr::litLongNode(m_prog, getLong(*args[0]) * getLong(*args[1]));
    }
    case prog::sym::FuncKind::DivLong: {
      assert(args.size() == 2);
      return prog::expr::litLongNode(m_prog, getLong(*args[0]) / getLong(*args[1]));
    }
    case prog::sym::FuncKind::RemLong: {
      assert(args.size() == 2);
      return prog::expr::litLongNode(m_prog, getLong(*args[0]) % getLong(*args[1]));
    }
    case prog::sym::FuncKind::NegateLong: {
      assert(args.size() == 1);
      return prog::expr::litLongNode(m_prog, -getLong(*args[0]));
    }
    case prog::sym::FuncKind::IncrementLong: {
      assert(args.size() == 1);
      return prog::expr::litLongNode(m_prog, getLong(*args[0]) + 1);
    }
    case prog::sym::FuncKind::DecrementLong: {
      assert(args.size() == 1);
      return prog::expr::litLongNode(m_prog, getLong(*args[0]) - 1);
    }
    case prog::sym::FuncKind::CheckEqLong: {
      assert(args.size() == 2);
      return prog::expr::litBoolNode(m_prog, getLong(*args[0]) == getLong(*args[1]));
    }
    case prog::sym::FuncKind::CheckNEqLong: {
      assert(args.size() == 2);
      return prog::expr::litBoolNode(m_prog, getLong(*args[0]) != getLong(*args[1]));
    }
    case prog::sym::FuncKind::CheckLeLong: {
      assert(args.size() == 2);
      return prog::expr::litBoolNode(m_prog, getLong(*args[0]) < getLong(*args[1]));
    }
    case prog::sym::FuncKind::CheckLeEqLong: {
      assert(args.size() == 2);
      return prog::expr::litBoolNode(m_prog, getLong(*args[0]) <= getLong(*args[1]));
    }
    case prog::sym::FuncKind::CheckGtLong: {
      assert(args.size() == 2);
      return prog::expr::litBoolNode(m_prog, getLong(*args[0]) > getLong(*args[1]));
    }
    case prog::sym::FuncKind::CheckGtEqLong: {
      assert(args.size() == 2);
      return prog::expr::litBoolNode(m_prog, getLong(*args[0]) >= getLong(*args[1]));
    }
    case prog::sym::FuncKind::ConvLongInt: {
      assert(args.size() == 1);
      return prog::expr::litIntNode(m_prog, static_cast<int32_t>(getLong(*args[0])));
    }
    case prog::sym::FuncKind::ConvLongString: {
      assert(args.size() == 1);
      auto ss = std::stringstream{};
      ss << getLong(*args[0]);
      return prog::expr::litStringNode(m_prog, ss.str());
    }
    case prog::sym::FuncKind::DefLong: {
      assert(args.size() == 0);
      return prog::expr::litLongNode(m_prog, 0);
    }

    // Char
    case prog::sym::FuncKind::InvBool: {
      assert(args.size() == 1);
      return prog::expr::litBoolNode(m_prog, !getBool(*args[0]));
    }
    case prog::sym::FuncKind::CheckEqBool: {
      assert(args.size() == 2);
      return prog::expr::litBoolNode(m_prog, getBool(*args[0]) == getBool(*args[1]));
    }
    case prog::sym::FuncKind::CheckNEqBool: {
      assert(args.size() == 2);
      return prog::expr::litBoolNode(m_prog, getBool(*args[0]) != getBool(*args[1]));
    }
    case prog::sym::FuncKind::ConvBoolString: {
      assert(args.size() == 1);
      return prog::expr::litStringNode(m_prog, getBool(*args[0]) ? "true" : "false");
    }
    case prog::sym::FuncKind::DefBool: {
      assert(args.size() == 0);
      return prog::expr::litBoolNode(m_prog, false);
    }

    // Char
    case prog::sym::FuncKind::CombineChar: {
      assert(args.size() == 2);
      auto result = std::string{};
      result += getChar(*args[0]);
      result += getChar(*args[1]);
      return prog::expr::litStringNode(m_prog, std::move(result));
    }
    case prog::sym::FuncKind::IncrementChar: {
      assert(args.size() == 1);
      return prog::expr::litCharNode(m_prog, static_cast<uint8_t>(getChar(*args[0]) + 1));
    }
    case prog::sym::FuncKind::DecrementChar: {
      assert(args.size() == 1);
      return prog::expr::litCharNode(m_prog, static_cast<uint8_t>(getChar(*args[0]) - 1));
    }
    case prog::sym::FuncKind::ConvCharString: {
      assert(args.size() == 1);
      auto c = getChar(*args[0]);
      return prog::expr::litStringNode(m_prog, std::string(1, c));
    }

    // String
    case prog::sym::FuncKind::AddString: {
      assert(args.size() == 2);
      return prog::expr::litStringNode(m_prog, getString(*args[0]) + getString(*args[1]));
    }
    case prog::sym::FuncKind::LengthString: {
      assert(args.size() == 1);
      return prog::expr::litIntNode(m_prog, static_cast<int32_t>(getString(*args[0]).size()));
    }
    case prog::sym::FuncKind::IndexString: {
      assert(args.size() == 2);
      auto str = getString(*args[0]);
      auto idx = getInt(*args[1]);
      if (idx < 0 || static_cast<unsigned>(idx) >= str.length()) {
        return prog::expr::litCharNode(m_prog, 0);
      }
      return prog::expr::litCharNode(m_prog, str[idx]);
    }
    case prog::sym::FuncKind::SliceString: {
      assert(args.size() == 3);
      auto str   = getString(*args[0]);
      auto start = getInt(*args[1]);
      auto end   = getInt(*args[2]);
      if (start < 0) {
        start = 0;
      }
      if (end < 0) {
        end = 0;
      }
      if (static_cast<unsigned>(end) >= str.length()) {
        end = str.length();
      }
      if (start > end) {
        start = end;
      }
      return prog::expr::litStringNode(m_prog, str.substr(start, end - start));
    }
    case prog::sym::FuncKind::AppendChar: {
      assert(args.size() == 2);
      auto result = getString(*args[0]);
      result += getChar(*args[1]);
      return prog::expr::litStringNode(m_prog, std::move(result));
    }
    case prog::sym::FuncKind::CheckEqString: {
      assert(args.size() == 2);
      return prog::expr::litBoolNode(m_prog, getString(*args[0]) == getString(*args[1]));
    }
    case prog::sym::FuncKind::CheckNEqString: {
      assert(args.size() == 2);
      return prog::expr::litBoolNode(m_prog, getString(*args[0]) != getString(*args[1]));
    }
    case prog::sym::FuncKind::DefString: {
      assert(args.size() == 0);
      return prog::expr::litStringNode(m_prog, "");
    }

    default:
      throw std::logic_error{"Unsupported func-kind"};
    }
  }

  [[nodiscard]] auto
  maybePrecomputeReinterpretConv(const prog::expr::Node& arg, prog::sym::TypeId dstType)
      -> prog::expr::NodePtr {

    assert(internal::isLiteral(arg));
    auto srcTypeDecl = m_prog.getTypeDecl(arg.getType());

    // Char to int.
    if (srcTypeDecl.getKind() == prog::sym::TypeKind::Char && dstType == m_prog.getInt()) {
      return prog::expr::litIntNode(m_prog, static_cast<int32_t>(getChar(arg)));
    }

    // Int as float.
    if (srcTypeDecl.getKind() == prog::sym::TypeKind::Int && dstType == m_prog.getFloat()) {
      auto src = getInt(arg);
      return prog::expr::litFloatNode(m_prog, reinterpret_cast<float&>(src));
    }

    // Float as int.
    if (srcTypeDecl.getKind() == prog::sym::TypeKind::Float && dstType == m_prog.getInt()) {
      auto src = getFloat(arg);
      return prog::expr::litIntNode(m_prog, reinterpret_cast<int&>(src));
    }

    // Enum to int.
    if (srcTypeDecl.getKind() == prog::sym::TypeKind::Enum && dstType == m_prog.getInt()) {
      const auto val = arg.downcast<prog::expr::LitEnumNode>()->getVal();
      return prog::expr::litIntNode(m_prog, val);
    }

    return nullptr;
  }
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

} // namespace opt
