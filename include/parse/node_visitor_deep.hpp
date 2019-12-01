#pragma once
#include "parse/node_visitor.hpp"

namespace parse {

class DeepNodeVisitor : public NodeVisitor {
public:
  auto visit(const ErrorNode& n) -> void override;
  auto visit(const BinaryExprNode& n) -> void override;
  auto visit(const CallExprNode& n) -> void override;
  auto visit(const ConditionalExprNode& n) -> void override;
  auto visit(const ConstDeclExprNode& n) -> void override;
  auto visit(const ConstExprNode& n) -> void override;
  auto visit(const FieldExprNode& n) -> void override;
  auto visit(const GroupExprNode& n) -> void override;
  auto visit(const LitExprNode& n) -> void override;
  auto visit(const ParenExprNode& n) -> void override;
  auto visit(const SwitchExprElseNode& n) -> void override;
  auto visit(const SwitchExprIfNode& n) -> void override;
  auto visit(const SwitchExprNode& n) -> void override;
  auto visit(const UnaryExprNode& n) -> void override;
  auto visit(const ExecStmtNode& n) -> void override;
  auto visit(const FuncDeclStmtNode& n) -> void override;
  auto visit(const StructDeclStmtNode& n) -> void override;
};

} // namespace parse
