#pragma once
#include "internal/context.hpp"
#include "prog/expr/node_visitor_optional.hpp"

namespace frontend::internal {

// Basic detection for infinite recursion, does not check all paths that could lead to infinite
// recursion but its a first guard against mistakes.
class CheckInfRecursion final : public prog::expr::OptionalNodeVisitor {
public:
  CheckInfRecursion(const Context& ctx, prog::sym::FuncId func);

  [[nodiscard]] auto isInfRecursion() const -> bool;

  auto visit(const prog::expr::AssignExprNode& n) -> void override;
  auto visit(const prog::expr::SwitchExprNode& n) -> void override;
  auto visit(const prog::expr::CallExprNode& n) -> void override;
  auto visit(const prog::expr::CallDynExprNode& n) -> void override;
  auto visit(const prog::expr::ClosureNode& n) -> void override;
  auto visit(const prog::expr::FieldExprNode& n) -> void override;
  auto visit(const prog::expr::GroupExprNode& n) -> void override;
  auto visit(const prog::expr::UnionCheckExprNode& n) -> void override;
  auto visit(const prog::expr::UnionGetExprNode& n) -> void override;

private:
  const Context& m_ctx;
  prog::sym::FuncId m_func;
  bool m_infRec;

  [[nodiscard]] auto exprLeadsToInfRec(const prog::expr::Node& expr) const -> bool;
};

} // namespace frontend::internal
