#pragma once
#include "parse/node_visitor.hpp"

namespace parse {

class OptionalNodeVisitor : public NodeVisitor {
public:
  auto visit(const ErrorNode & /*unused*/) -> void override {}
  auto visit(const BinaryExprNode & /*unused*/) -> void override {}
  auto visit(const CallExprNode & /*unused*/) -> void override {}
  auto visit(const ConstDeclExprNode & /*unused*/) -> void override {}
  auto visit(const ConstExprNode & /*unused*/) -> void override {}
  auto visit(const GroupExprNode & /*unused*/) -> void override {}
  auto visit(const LitExprNode & /*unused*/) -> void override {}
  auto visit(const ParenExprNode & /*unused*/) -> void override {}
  auto visit(const SwitchExprElseNode & /*unused*/) -> void override {}
  auto visit(const SwitchExprIfNode & /*unused*/) -> void override {}
  auto visit(const SwitchExprNode & /*unused*/) -> void override {}
  auto visit(const UnaryExprNode & /*unused*/) -> void override {}
  auto visit(const ExecStmtNode & /*unused*/) -> void override {}
  auto visit(const FuncDeclStmtNode & /*unused*/) -> void override {}
};

} // namespace parse
