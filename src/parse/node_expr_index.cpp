#include "parse/node_expr_index.hpp"
#include "utilities.hpp"

namespace parse {

IndexExprNode::IndexExprNode(
    NodePtr lhs,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) :
    m_lhs{std::move(lhs)},
    m_open{std::move(open)},
    m_args{std::move(args)},
    m_commas{std::move(commas)},
    m_close{std::move(close)} {}

auto IndexExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const IndexExprNode*>(&rhs);
  return r != nullptr && *m_lhs == *r->m_lhs && nodesEqual(m_args, r->m_args);
}

auto IndexExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !IndexExprNode::operator==(rhs);
}

auto IndexExprNode::operator[](unsigned int i) const -> const Node& {
  if (i == 0) {
    return *m_lhs;
  }
  if (i > m_args.size()) {
    throw std::out_of_range{"No child at given index"};
  }
  return *m_args[i - 1];
}

auto IndexExprNode::getChildCount() const -> unsigned int { return m_args.size() + 1; }

auto IndexExprNode::getSpan() const -> input::Span {
  return input::Span::combine(m_lhs->getSpan(), m_close.getSpan());
}

auto IndexExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto IndexExprNode::print(std::ostream& out) const -> std::ostream& { return out << "[]"; }

// Factories.
auto indexExprNode(
    NodePtr lhs,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) -> NodePtr {
  if (lhs == nullptr) {
    throw std::invalid_argument{"lhs cannot be null"};
  }
  if (args.empty()) {
    throw std::invalid_argument{"args cannot be empty"};
  }
  if (anyNodeNull(args)) {
    throw std::invalid_argument{"args cannot contain a nullptr"};
  }
  if (commas.size() != args.size() - 1) {
    throw std::invalid_argument{"Incorrect number of commas"};
  }
  return std::unique_ptr<IndexExprNode>{new IndexExprNode{
      std::move(lhs), std::move(open), std::move(args), std::move(commas), std::move(close)}};
}

} // namespace parse
