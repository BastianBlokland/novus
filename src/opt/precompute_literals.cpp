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

[[nodiscard]] static auto isSupported(prog::sym::FuncKind funcKind) -> bool {
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

class PrecomputeRewriter final : public prog::expr::Rewriter {
public:
  PrecomputeRewriter(
      const prog::Program& prog, prog::sym::FuncId funcId, prog::sym::ConstDeclTable* consts) :
      m_prog{prog}, m_funcId{funcId}, m_consts{consts} {

    if (m_consts == nullptr) {
      throw std::invalid_argument{"Consts table cannot be null"};
    }
  }

  auto rewrite(const prog::expr::Node& expr) -> prog::expr::NodePtr override {
    /* Rewrite calls to intrinsic computations (for example integer negation) to which all
    arguments are literals. */

    if (expr.getKind() == prog::expr::NodeKind::Call) {
      const auto* callExpr = expr.downcast<prog::expr::CallExprNode>();
      const auto funcId    = callExpr->getFunc();
      const auto funcKind  = m_prog.getFuncDecl(funcId).getKind();

      // Check if the call is a operation we support.
      if (isSupported(funcKind)) {
        /* Now we need to check if all the arguments are literals, to support precomputing nested
         * calls we rewrite the arguments first. */

        auto newArgs = std::vector<prog::expr::NodePtr>{};
        newArgs.reserve(expr.getChildCount());
        for (auto i = 0U; i != expr.getChildCount(); ++i) {
          newArgs.push_back(rewrite(expr[i]));
        }

        // If all the arguments are literals we can precompute the value.
        if (std::all_of(newArgs.begin(), newArgs.end(), [](const prog::expr::NodePtr& n) {
              return internal::isLiteral(*n);
            })) {
          return precompute(funcKind, std::move(newArgs));
        } else {
          // if we cannot precompute then construct a copy of the call.
          return prog::expr::callExprNode(m_prog, funcId, std::move(newArgs));
        }
      }
    }

    // If we don't want to rewrite this expression just make a clone.
    return expr.clone(this);
  }

private:
  const prog::Program& m_prog;
  prog::sym::FuncId m_funcId;
  prog::sym::ConstDeclTable* m_consts;

  [[nodiscard]] auto precompute(prog::sym::FuncKind funcKind, std::vector<prog::expr::NodePtr> args)
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
};

auto precomputeLiterals(const prog::Program& prog) -> prog::Program {
  return internal::rewrite(
      prog,
      [](const prog::Program& prog, prog::sym::FuncId funcId, prog::sym::ConstDeclTable* consts) {
        return std::make_unique<PrecomputeRewriter>(prog, funcId, consts);
      });
}

} // namespace opt
