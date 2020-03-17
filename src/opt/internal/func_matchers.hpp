#pragma once
#include "prog/expr/node_visitor_deep.hpp"
#include "prog/program.hpp"

namespace opt::internal {

template <typename UnaryPredicate>
class ExprMatcher final : public prog::expr::DeepNodeVisitor {
  static_assert(std::is_invocable_r<bool, UnaryPredicate, const prog::expr::Node&>::value);

public:
  explicit ExprMatcher(UnaryPredicate pred) : m_pred{pred}, m_found{false} {}

  [[nodiscard]] auto isFound() const { return m_found; }

protected:
  auto visitNode(const prog::expr::Node* n) -> void override {
    if (m_pred(*n)) {
      m_found = true;
    } else {
      prog::expr::DeepNodeVisitor::visitNode(n);
    }
  }

private:
  UnaryPredicate m_pred;
  bool m_found;
};

template <typename UnaryPredicate>
auto matchesExpr(const prog::Program& prog, prog::sym::FuncId funcId, UnaryPredicate predicate) {
  const auto& funcDecl = prog.getFuncDecl(funcId);

  // Non-user funcs have no expressions.
  if (funcDecl.getKind() != prog::sym::FuncKind::User) {
    return false;
  }

  const auto& funcDef = prog.getFuncDef(funcId);
  auto matcher        = ExprMatcher{predicate};
  funcDef.getExpr().accept(&matcher);

  return matcher.isFound();
}

auto isUserFunc(const prog::Program& prog, prog::sym::FuncId funcId) -> bool;

auto isRecursive(const prog::Program& prog, prog::sym::FuncId funcId) -> bool;

} // namespace opt::internal
