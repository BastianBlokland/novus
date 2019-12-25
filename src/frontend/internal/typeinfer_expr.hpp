#pragma once
#include "internal/func_template_table.hpp"
#include <unordered_map>

namespace frontend::internal {

class TypeInferExpr final : public parse::NodeVisitor {
public:
  TypeInferExpr() = delete;
  TypeInferExpr(
      Context* context,
      const TypeSubstitutionTable* typeSubTable,
      std::unordered_map<std::string, prog::sym::TypeId>* constTypes,
      bool aggressive);

  [[nodiscard]] auto getInferredType() const noexcept -> prog::sym::TypeId;

  auto visit(const parse::CommentNode& n) -> void override;
  auto visit(const parse::ErrorNode& n) -> void override;
  auto visit(const parse::BinaryExprNode& n) -> void override;
  auto visit(const parse::CallExprNode& n) -> void override;
  auto visit(const parse::ConditionalExprNode& n) -> void override;
  auto visit(const parse::ConstDeclExprNode& n) -> void override;
  auto visit(const parse::ConstExprNode& n) -> void override;
  auto visit(const parse::FieldExprNode& n) -> void override;
  auto visit(const parse::GroupExprNode& n) -> void override;
  auto visit(const parse::IndexExprNode& n) -> void override;
  auto visit(const parse::IsExprNode& n) -> void override;
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
  Context* m_context;
  const TypeSubstitutionTable* m_typeSubTable;
  std::unordered_map<std::string, prog::sym::TypeId>* m_constTypes;
  bool m_aggressive;
  prog::sym::TypeId m_type;

  auto inferSubExpr(const parse::Node& n) -> prog::sym::TypeId;
  auto inferFuncCall(const std::string& funcName, const prog::sym::TypeSet& argTypes)
      -> prog::sym::TypeId;

  auto setConstType(const lex::Token& constId, prog::sym::TypeId type) -> void;
  auto inferConstType(const lex::Token& constId) -> prog::sym::TypeId;

  [[nodiscard]] auto isType(const std::string& name) const -> bool;
};

} // namespace frontend::internal
