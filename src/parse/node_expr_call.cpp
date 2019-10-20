#include "parse/node_expr_call.hpp"

namespace parse {

CallExprNode::CallExprNode(
    lex::Token id,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) :

    Node(NodeType::ExprCall),
    m_id{std::move(id)},
    m_open{std::move(open)},
    m_args{std::move(args)},
    m_commas{std::move(commas)},
    m_close{std::move(close)} {

  if (std::any_of(m_args.begin(), m_args.end(), [](const NodePtr& p) { return p == nullptr; })) {
    throw std::invalid_argument("args cannot contain a nullptr");
  }
  if (m_args.empty() ? !m_commas.empty() : m_commas.size() != m_args.size() - 1) {
    throw std::invalid_argument("Incorrect number of commas");
  }
}

auto CallExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const CallExprNode*>(&rhs);
  return r != nullptr && m_id == r->m_id && m_args.size() == r->m_args.size() &&
      std::equal(
             m_args.begin(),
             m_args.end(),
             r->m_args.begin(),
             [](const NodePtr& l, const NodePtr& r) { return *l == *r; });
}

auto CallExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !CallExprNode::operator==(rhs);
}

auto CallExprNode::operator[](int i) const -> const Node& {
  if (i < 0 || static_cast<unsigned>(i) >= m_args.size()) {
    throw std::out_of_range("No child at given index");
  }
  return *m_args[i];
}

auto CallExprNode::getChildCount() const -> int { return m_args.size(); }

auto CallExprNode::print(std::ostream& out) const -> std::ostream& {
  return out << "call-" << m_id;
}

// Factories.
auto callExprNode(
    lex::Token id,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) -> NodePtr {
  return std::make_unique<CallExprNode>(
      std::move(id), std::move(open), std::move(args), std::move(commas), std::move(close));
}

} // namespace parse
