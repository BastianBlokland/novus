#pragma once
#include "prog/expr/node_visitor_deep.hpp"
#include "prog/program.hpp"

namespace opt::internal {

template <typename UnaryPredicate, bool StopAtFirstHit = true>
class ExprMatcher final : public prog::expr::DeepNodeVisitor {
  static_assert(std::is_invocable_r<bool, UnaryPredicate, const prog::expr::Node&>::value);

public:
  explicit ExprMatcher(UnaryPredicate pred) : m_pred{pred}, m_count{0} {}

  [[nodiscard]] auto isFound() const { return m_count != 0; }
  [[nodiscard]] auto getCount() const { return m_count; }

protected:
  auto visitNode(const prog::expr::Node* n) -> void override {
    if (m_pred(*n)) {
      ++m_count;
    }
    if (m_count == 0 || !StopAtFirstHit) {
      prog::expr::DeepNodeVisitor::visitNode(n);
    }
  }

private:
  UnaryPredicate m_pred;
  unsigned int m_count;
};

template <typename UnaryPredicate, bool StopAtFirstHit = true>
auto matchesExpr(const prog::Program& prog, prog::sym::FuncId funcId, UnaryPredicate predicate)
    -> unsigned int {
  const auto& funcDecl = prog.getFuncDecl(funcId);

  // Non-user funcs have no expressions.
  if (funcDecl.getKind() != prog::sym::FuncKind::User) {
    return false;
  }

  const auto& funcDef = prog.getFuncDef(funcId);
  auto matcher        = ExprMatcher<UnaryPredicate, StopAtFirstHit>{predicate};
  funcDef.getBody().accept(&matcher);

  return matcher.getCount();
}

auto isRecursive(const prog::Program& prog, const prog::sym::FuncDef& funcDef) -> bool;

auto hasFuncDefFlags(
    const prog::Program& prog, prog::sym::FuncId funcId, prog::sym::FuncDef::Flags flags) -> bool;

auto hasSideEffect(const prog::Program& prog, const prog::expr::Node& expr) -> bool;

auto isLiteral(const prog::expr::Node& expr) -> bool;

auto exprSize(const prog::expr::Node& expr) -> unsigned int;

} // namespace opt::internal
