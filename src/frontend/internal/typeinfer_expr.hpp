#pragma once
#include "frontend/diag.hpp"
#include "prog/program.hpp"
#include <unordered_map>

namespace frontend::internal {

class TypeInferExpr final : public parse::NodeVisitor {
public:
  TypeInferExpr() = delete;
  TypeInferExpr(
      prog::Program* prog, std::unordered_map<std::string, prog::sym::TypeId>* constTypes);

  [[nodiscard]] auto getType() const noexcept -> prog::sym::TypeId;

  auto visit(const parse::ErrorNode& n) -> void override;
  auto visit(const parse::BinaryExprNode& n) -> void override;
  auto visit(const parse::CallExprNode& n) -> void override;
  auto visit(const parse::ConditionalExprNode& n) -> void override;
  auto visit(const parse::ConstDeclExprNode& n) -> void override;
  auto visit(const parse::ConstExprNode& n) -> void override;
  auto visit(const parse::FieldExprNode& n) -> void override;
  auto visit(const parse::GroupExprNode& n) -> void override;
  auto visit(const parse::LitExprNode& n) -> void override;
  auto visit(const parse::ParenExprNode& n) -> void override;
  auto visit(const parse::SwitchExprElseNode& n) -> void override;
  auto visit(const parse::SwitchExprIfNode& n) -> void override;
  auto visit(const parse::SwitchExprNode& n) -> void override;
  auto visit(const parse::UnaryExprNode& n) -> void override;
  auto visit(const parse::ExecStmtNode& n) -> void override;
  auto visit(const parse::FuncDeclStmtNode& n) -> void override;
  auto visit(const parse::StructDeclStmtNode& n) -> void override;
  auto visit(const parse::UnionDeclStmtNode& n) -> void override;

private:
  prog::Program* m_prog;
  std::unordered_map<std::string, prog::sym::TypeId>* m_constTypes;
  prog::sym::TypeId m_type;

  auto inferSubExpr(const parse::Node& n) -> prog::sym::TypeId;
  auto inferFuncCall(const std::string& funcName, std::vector<prog::sym::TypeId> argTypes)
      -> prog::sym::TypeId;

  auto setConstType(const lex::Token& constId, prog::sym::TypeId type) -> void;
  auto inferConstType(const lex::Token& constId) -> prog::sym::TypeId;
};

} // namespace frontend::internal
