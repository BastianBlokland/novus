#include "parse/node_visitor_deep.hpp"
#include "parse/nodes.hpp"

namespace parse {

static auto visitChildren(const Node* n, DeepNodeVisitor* visitor) {
  for (auto i = 0U; i < n->getChildCount(); ++i) {
    n->operator[](i).accept(visitor);
  }
}

auto DeepNodeVisitor::visit(const CommentNode& n) -> void { visitChildren(&n, this); }

auto DeepNodeVisitor::visit(const ErrorNode& n) -> void { visitChildren(&n, this); }

auto DeepNodeVisitor::visit(const AnonFuncExprNode& n) -> void { visitChildren(&n, this); }

auto DeepNodeVisitor::visit(const BinaryExprNode& n) -> void { visitChildren(&n, this); }

auto DeepNodeVisitor::visit(const CallExprNode& n) -> void { visitChildren(&n, this); }

auto DeepNodeVisitor::visit(const ConditionalExprNode& n) -> void { visitChildren(&n, this); }

auto DeepNodeVisitor::visit(const ConstDeclExprNode& n) -> void { visitChildren(&n, this); }

auto DeepNodeVisitor::visit(const IdExprNode& n) -> void { visitChildren(&n, this); }

auto DeepNodeVisitor::visit(const FieldExprNode& n) -> void { visitChildren(&n, this); }

auto DeepNodeVisitor::visit(const GroupExprNode& n) -> void { visitChildren(&n, this); }

auto DeepNodeVisitor::visit(const IndexExprNode& n) -> void { visitChildren(&n, this); }

auto DeepNodeVisitor::visit(const IsExprNode& n) -> void { visitChildren(&n, this); }

auto DeepNodeVisitor::visit(const LitExprNode& n) -> void { visitChildren(&n, this); }

auto DeepNodeVisitor::visit(const ParenExprNode& n) -> void { visitChildren(&n, this); }

auto DeepNodeVisitor::visit(const SwitchExprElseNode& n) -> void { visitChildren(&n, this); }

auto DeepNodeVisitor::visit(const SwitchExprIfNode& n) -> void { visitChildren(&n, this); }

auto DeepNodeVisitor::visit(const SwitchExprNode& n) -> void { visitChildren(&n, this); }

auto DeepNodeVisitor::visit(const UnaryExprNode& n) -> void { visitChildren(&n, this); }

auto DeepNodeVisitor::visit(const EnumDeclStmtNode& n) -> void { visitChildren(&n, this); }

auto DeepNodeVisitor::visit(const ExecStmtNode& n) -> void { visitChildren(&n, this); }

auto DeepNodeVisitor::visit(const FuncDeclStmtNode& n) -> void { visitChildren(&n, this); }

auto DeepNodeVisitor::visit(const ImportStmtNode& n) -> void { visitChildren(&n, this); }

auto DeepNodeVisitor::visit(const StructDeclStmtNode& n) -> void { visitChildren(&n, this); }

auto DeepNodeVisitor::visit(const UnionDeclStmtNode& n) -> void { visitChildren(&n, this); }

} // namespace parse
