#pragma once
#include "frontend/diag.hpp"
#include "frontend/source.hpp"
#include "prog/program.hpp"

namespace frontend::internal {

class GetExpr final : public parse::NodeVisitor {
public:
  GetExpr() = delete;
  GetExpr(
      const Source& src,
      prog::Program* prog,
      prog::sym::ConstDeclTable* consts,
      std::vector<prog::sym::ConstId>* visibleConsts,
      bool checkedConstsAccess = false);

  [[nodiscard]] auto hasErrors() const noexcept -> bool;
  [[nodiscard]] auto getDiags() const noexcept -> const std::vector<Diag>&;
  [[nodiscard]] auto getValue() -> prog::expr::NodePtr&;

  auto visit(const parse::ErrorNode& n) -> void override;
  auto visit(const parse::BinaryExprNode& n) -> void override;
  auto visit(const parse::CallExprNode& n) -> void override;
  auto visit(const parse::ConditionalExprNode& n) -> void override;
  auto visit(const parse::ConstDeclExprNode& n) -> void override;
  auto visit(const parse::ConstExprNode& n) -> void override;
  auto visit(const parse::FieldExprNode& n) -> void override;
  auto visit(const parse::GroupExprNode& n) -> void override;
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
  enum class BinLogicOp { And, Or };

  const Source& m_src;
  prog::Program* m_prog;
  prog::sym::ConstDeclTable* m_consts;
  std::vector<prog::sym::ConstId>* m_visibleConsts;
  bool m_checkedConstsAccess;
  std::vector<Diag> m_diags;
  prog::expr::NodePtr m_expr;

  auto getSubExpr(
      const parse::Node& n,
      std::vector<prog::sym::ConstId>* visibleConsts,
      bool checkedConstsAccess = false) -> prog::expr::NodePtr;

  auto getBinLogicOpExpr(const parse::BinaryExprNode& n, BinLogicOp op) -> prog::expr::NodePtr;

  auto declareConst(const lex::Token& nameToken, prog::sym::TypeId type)
      -> std::optional<prog::sym::ConstId>;

  auto isBoolType(prog::sym::TypeId type) -> bool;
};

} // namespace frontend::internal
