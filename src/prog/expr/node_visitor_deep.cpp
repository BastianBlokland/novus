#include "prog/expr/node_visitor_deep.hpp"
#include "prog/expr/nodes.hpp"

namespace prog::expr {

auto DeepNodeVisitor::visit(const AssignExprNode& n) -> void { visitNode(&n); }

auto DeepNodeVisitor::visit(const SwitchExprNode& n) -> void { visitNode(&n); }

auto DeepNodeVisitor::visit(const CallExprNode& n) -> void { visitNode(&n); }

auto DeepNodeVisitor::visit(const CallDynExprNode& n) -> void { visitNode(&n); }

auto DeepNodeVisitor::visit(const CallSelfExprNode& n) -> void { visitNode(&n); }

auto DeepNodeVisitor::visit(const ClosureNode& n) -> void { visitNode(&n); }

auto DeepNodeVisitor::visit(const ConstExprNode& n) -> void { visitNode(&n); }

auto DeepNodeVisitor::visit(const FieldExprNode& n) -> void { visitNode(&n); }

auto DeepNodeVisitor::visit(const GroupExprNode& n) -> void { visitNode(&n); }

auto DeepNodeVisitor::visit(const UnionCheckExprNode& n) -> void { visitNode(&n); }

auto DeepNodeVisitor::visit(const UnionGetExprNode& n) -> void { visitNode(&n); }

auto DeepNodeVisitor::visit(const FailNode& n) -> void { visitNode(&n); }

auto DeepNodeVisitor::visit(const LitBoolNode& n) -> void { visitNode(&n); }

auto DeepNodeVisitor::visit(const LitFloatNode& n) -> void { visitNode(&n); }

auto DeepNodeVisitor::visit(const LitFuncNode& n) -> void { visitNode(&n); }

auto DeepNodeVisitor::visit(const LitIntNode& n) -> void { visitNode(&n); }

auto DeepNodeVisitor::visit(const LitLongNode& n) -> void { visitNode(&n); }

auto DeepNodeVisitor::visit(const LitStringNode& n) -> void { visitNode(&n); }

auto DeepNodeVisitor::visit(const LitCharNode& n) -> void { visitNode(&n); }

auto DeepNodeVisitor::visit(const LitEnumNode& n) -> void { visitNode(&n); }

auto DeepNodeVisitor::visitNode(const Node* n) -> void {
  for (auto i = 0U; i < n->getChildCount(); ++i) {
    n->operator[](i).accept(this);
  }
}

} // namespace prog::expr
