#pragma once
#include "prog/expr/node_visitor.hpp"

namespace prog::expr {

class OptionalNodeVisitor : public NodeVisitor {
public:
  auto visit(const AssignExprNode & /*unused*/) -> void override {}
  auto visit(const SwitchExprNode & /*unused*/) -> void override {}
  auto visit(const CallExprNode & /*unused*/) -> void override {}
  auto visit(const CallDynExprNode & /*unused*/) -> void override {}
  auto visit(const CallSelfExprNode & /*unused*/) -> void override {}
  auto visit(const ClosureNode & /*unused*/) -> void override {}
  auto visit(const ConstExprNode & /*unused*/) -> void override {}
  auto visit(const FieldExprNode & /*unused*/) -> void override {}
  auto visit(const GroupExprNode & /*unused*/) -> void override {}
  auto visit(const UnionCheckExprNode & /*unused*/) -> void override {}
  auto visit(const UnionGetExprNode & /*unused*/) -> void override {}
  auto visit(const FailNode & /*unused*/) -> void override {}
  auto visit(const LitBoolNode & /*unused*/) -> void override {}
  auto visit(const LitFloatNode & /*unused*/) -> void override {}
  auto visit(const LitFuncNode & /*unused*/) -> void override {}
  auto visit(const LitIntNode & /*unused*/) -> void override {}
  auto visit(const LitLongNode & /*unused*/) -> void override {}
  auto visit(const LitStringNode & /*unused*/) -> void override {}
  auto visit(const LitCharNode & /*unused*/) -> void override {}
  auto visit(const LitEnumNode & /*unused*/) -> void override {}
};

} // namespace prog::expr
