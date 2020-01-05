#pragma once
#include "backend/builder.hpp"
#include "prog/expr/node_visitor.hpp"
#include "prog/program.hpp"

namespace backend::internal {

class GenExpr final : public prog::expr::NodeVisitor {
public:
  GenExpr(const prog::Program& program, Builder* builder);

  auto visit(const prog::expr::AssignExprNode& n) -> void override;
  auto visit(const prog::expr::SwitchExprNode& n) -> void override;
  auto visit(const prog::expr::CallExprNode& n) -> void override;
  auto visit(const prog::expr::CallDynExprNode& n) -> void override;
  auto visit(const prog::expr::ClosureNode& n) -> void override;
  auto visit(const prog::expr::ConstExprNode& n) -> void override;
  auto visit(const prog::expr::FieldExprNode& n) -> void override;
  auto visit(const prog::expr::GroupExprNode& n) -> void override;
  auto visit(const prog::expr::UnionCheckExprNode& n) -> void override;
  auto visit(const prog::expr::UnionGetExprNode& n) -> void override;
  auto visit(const prog::expr::FailNode& n) -> void override;
  auto visit(const prog::expr::LitBoolNode& n) -> void override;
  auto visit(const prog::expr::LitFloatNode& n) -> void override;
  auto visit(const prog::expr::LitFuncNode& n) -> void override;
  auto visit(const prog::expr::LitIntNode& n) -> void override;
  auto visit(const prog::expr::LitStringNode& n) -> void override;

private:
  const prog::Program& m_program;
  Builder* m_builder;

  auto genSubExpr(const prog::expr::Node& n) -> void;
};

} // namespace backend::internal
