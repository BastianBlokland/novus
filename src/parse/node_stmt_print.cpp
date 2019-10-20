#include "parse/node_stmt_print.hpp"

namespace parse {

PrintStmtNode::PrintStmtNode(lex::Token kw, NodePtr expr) :
    Node(NodeType::StmtPrint), m_kw{std::move(kw)}, m_expr{std::move(expr)} {

  if (m_expr == nullptr) {
    throw std::invalid_argument("Expression cannot be null");
  }
}

auto PrintStmtNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const PrintStmtNode*>(&rhs);
  return r != nullptr && *m_expr == *r->m_expr;
}

auto PrintStmtNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !PrintStmtNode::operator==(rhs);
}

auto PrintStmtNode::operator[](int i) const -> const Node& {
  if (i == 0) {
    return *m_expr;
  }
  throw std::out_of_range("No child at given index");
}

auto PrintStmtNode::getChildCount() const -> int { return 1; }

auto PrintStmtNode::print(std::ostream& out) const -> std::ostream& { return out << "print"; }

// Factories.
auto printStmtNode(lex::Token kw, NodePtr expr) -> NodePtr {
  return std::make_unique<PrintStmtNode>(std::move(kw), std::move(expr));
}

} // namespace parse
