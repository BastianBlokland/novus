#include "internal/expr_matchers.hpp"
#include "internal/prog_rewrite.hpp"
#include "opt/opt.hpp"
#include "prog/expr/nodes.hpp"
#include "prog/expr/rewriter.hpp"
#include <algorithm>
#include <cassert>

namespace opt {

static auto getInt(const prog::expr::Node& n) -> int32_t {
  return n.downcast<prog::expr::LitIntNode>()->getVal();
}

static auto getFloat(const prog::expr::Node& n) -> float {
  return n.downcast<prog::expr::LitFloatNode>()->getVal();
}

static auto getLong(const prog::expr::Node& n) -> int64_t {
  return n.downcast<prog::expr::LitLongNode>()->getVal();
}

static auto getChar(const prog::expr::Node& n) -> uint8_t {
  return n.downcast<prog::expr::LitCharNode>()->getVal();
}

static auto getString(const prog::expr::Node& n) -> std::string {
  return n.downcast<prog::expr::LitStringNode>()->getVal();
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
    /* Rewrite calls to a certain set of computations (for example integer negation) to which all
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

  [[nodiscard]] auto isSupported(prog::sym::FuncKind funcKind) -> bool {
    switch (funcKind) {
    // Int
    case prog::sym::FuncKind::NegateInt:
    case prog::sym::FuncKind::ConvIntChar:

    // Float
    case prog::sym::FuncKind::NegateFloat:

    // Long
    case prog::sym::FuncKind::NegateLong:

    // Char
    case prog::sym::FuncKind::CombineChar:
    case prog::sym::FuncKind::ConvCharString:

    // String
    case prog::sym::FuncKind::AddString:
    case prog::sym::FuncKind::AppendChar:

      return true;

    default:
      return false;
    }
  }

  [[nodiscard]] auto precompute(prog::sym::FuncKind funcKind, std::vector<prog::expr::NodePtr> args)
      -> prog::expr::NodePtr {
    switch (funcKind) {

    // Int
    case prog::sym::FuncKind::NegateInt: {
      assert(args.size() == 1);
      return prog::expr::litIntNode(m_prog, -getInt(*args[0]));
    }
    case prog::sym::FuncKind::ConvIntChar: {
      assert(args.size() == 1);
      return prog::expr::litCharNode(m_prog, static_cast<uint8_t>(getInt(*args[0])));
    }

    // Float
    case prog::sym::FuncKind::NegateFloat: {
      assert(args.size() == 1);
      return prog::expr::litFloatNode(m_prog, -getFloat(*args[0]));
    }

    // Long
    case prog::sym::FuncKind::NegateLong: {
      assert(args.size() == 1);
      return prog::expr::litLongNode(m_prog, -getLong(*args[0]));
    }

    // Char
    case prog::sym::FuncKind::CombineChar: {
      assert(args.size() == 2);
      auto result = std::string{};
      result += getChar(*args[0]);
      result += getChar(*args[1]);
      return prog::expr::litStringNode(m_prog, std::move(result));
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
    case prog::sym::FuncKind::AppendChar: {
      assert(args.size() == 2);
      auto result = getString(*args[0]);
      result += getChar(*args[1]);
      return prog::expr::litStringNode(m_prog, std::move(result));
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
