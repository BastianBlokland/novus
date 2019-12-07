#pragma once
#include "prog/expr/node_visitor.hpp"
#include "prog/program.hpp"
#include <optional>

namespace frontend::internal {

class CheckUnionExhaustiveness final : public prog::expr::NodeVisitor {
public:
  explicit CheckUnionExhaustiveness(const prog::Program& program);

  [[nodiscard]] auto isExhaustive() const -> bool;

  auto visit(const prog::expr::AssignExprNode& n) -> void override;
  auto visit(const prog::expr::SwitchExprNode& n) -> void override;
  auto visit(const prog::expr::CallExprNode& n) -> void override;
  auto visit(const prog::expr::ConstExprNode& n) -> void override;
  auto visit(const prog::expr::FieldExprNode& n) -> void override;
  auto visit(const prog::expr::GroupExprNode& n) -> void override;
  auto visit(const prog::expr::UnionCheckExprNode& n) -> void override;
  auto visit(const prog::expr::UnionGetExprNode& n) -> void override;
  auto visit(const prog::expr::FailNode& n) -> void override;
  auto visit(const prog::expr::LitBoolNode& n) -> void override;
  auto visit(const prog::expr::LitFloatNode& n) -> void override;
  auto visit(const prog::expr::LitIntNode& n) -> void override;
  auto visit(const prog::expr::LitStringNode& n) -> void override;

private:
  const prog::Program& m_program;
  std::optional<prog::sym::TypeId> m_unionType;
  std::vector<prog::sym::TypeId> m_checkedTypes;
};

} // namespace frontend::internal
