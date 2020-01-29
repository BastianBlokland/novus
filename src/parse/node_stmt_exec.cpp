#include "parse/node_stmt_exec.hpp"

namespace parse {

ExecStmtNode::ExecStmtNode(NodePtr callExpr) : m_callExpr{std::move(callExpr)} {}

auto ExecStmtNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const ExecStmtNode*>(&rhs);
  return r != nullptr && *m_callExpr == *r->m_callExpr;
}

auto ExecStmtNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !ExecStmtNode::operator==(rhs);
}

auto ExecStmtNode::operator[](unsigned int i) const -> const Node& {
  if (i == 0U) {
    return *m_callExpr;
  }
  throw std::out_of_range("No child at given index");
}

auto ExecStmtNode::getChildCount() const -> unsigned int { return 1U; }

auto ExecStmtNode::getSpan() const -> input::Span { return m_callExpr->getSpan(); }

auto ExecStmtNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto ExecStmtNode::print(std::ostream& out) const -> std::ostream& { return out << "exec"; }

// Factories.
auto execStmtNode(NodePtr callExpr) -> NodePtr {
  if (callExpr == nullptr) {
    throw std::invalid_argument{"Call expression cannot be null"};
  }
  return std::unique_ptr<ExecStmtNode>{new ExecStmtNode{std::move(callExpr)}};
}

} // namespace parse
