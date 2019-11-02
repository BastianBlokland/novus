#include "parse/node_visitor_reqursive.hpp"
#include "parse/nodes.hpp"

namespace parse {

static auto visitChildren(const Node* n, RecursiveNodeVisitor* visitor) {
  for (auto i = 0U; i < n->getChildCount(); ++i) {
    n->operator[](i).accept(visitor);
  }
}

auto RecursiveNodeVisitor::visit(const ErrorNode& n) -> void { visitChildren(&n, this); }

auto RecursiveNodeVisitor::visit(const BinaryExprNode& n) -> void { visitChildren(&n, this); }

auto RecursiveNodeVisitor::visit(const CallExprNode& n) -> void { visitChildren(&n, this); }

auto RecursiveNodeVisitor::visit(const ConstDeclExprNode& n) -> void { visitChildren(&n, this); }

auto RecursiveNodeVisitor::visit(const ConstExprNode& n) -> void { visitChildren(&n, this); }

auto RecursiveNodeVisitor::visit(const GroupExprNode& n) -> void { visitChildren(&n, this); }

auto RecursiveNodeVisitor::visit(const LitExprNode& n) -> void { visitChildren(&n, this); }

auto RecursiveNodeVisitor::visit(const ParenExprNode& n) -> void { visitChildren(&n, this); }

auto RecursiveNodeVisitor::visit(const SwitchExprElseNode& n) -> void { visitChildren(&n, this); }

auto RecursiveNodeVisitor::visit(const SwitchExprIfNode& n) -> void { visitChildren(&n, this); }

auto RecursiveNodeVisitor::visit(const SwitchExprNode& n) -> void { visitChildren(&n, this); }

auto RecursiveNodeVisitor::visit(const UnaryExprNode& n) -> void { visitChildren(&n, this); }

auto RecursiveNodeVisitor::visit(const ExecStmtNode& n) -> void { visitChildren(&n, this); }

auto RecursiveNodeVisitor::visit(const FuncDeclStmtNode& n) -> void { visitChildren(&n, this); }

} // namespace parse
