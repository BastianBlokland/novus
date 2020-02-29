#pragma once
#include "prog/expr/node_visitor.hpp"

namespace prog::expr {

class Node;

class DeepNodeVisitor : public NodeVisitor {
public:
  auto visit(const AssignExprNode& n) -> void override;
  auto visit(const SwitchExprNode& n) -> void override;
  auto visit(const CallExprNode& n) -> void override;
  auto visit(const CallDynExprNode& n) -> void override;
  auto visit(const CallSelfExprNode& n) -> void override;
  auto visit(const ClosureNode& n) -> void override;
  auto visit(const ConstExprNode& n) -> void override;
  auto visit(const FieldExprNode& n) -> void override;
  auto visit(const GroupExprNode& n) -> void override;
  auto visit(const UnionCheckExprNode& n) -> void override;
  auto visit(const UnionGetExprNode& n) -> void override;
  auto visit(const FailNode& n) -> void override;
  auto visit(const LitBoolNode& n) -> void override;
  auto visit(const LitFloatNode& n) -> void override;
  auto visit(const LitFuncNode& n) -> void override;
  auto visit(const LitIntNode& n) -> void override;
  auto visit(const LitLongNode& n) -> void override;
  auto visit(const LitStringNode& n) -> void override;
  auto visit(const LitCharNode& n) -> void override;
  auto visit(const LitEnumNode& n) -> void override;

protected:
  virtual auto visitNode(const Node* n) -> void;
};

} // namespace prog::expr
