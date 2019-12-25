#include "parse/node_comment.hpp"
#include "utilities.hpp"

namespace parse {

CommentNode::CommentNode(lex::Token comment) : m_comment{std::move(comment)} {}

auto CommentNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const CommentNode*>(&rhs);
  return r != nullptr && m_comment == r->m_comment;
}

auto CommentNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !CommentNode::operator==(rhs);
}

auto CommentNode::operator[](unsigned int /* unused */) const -> const Node& {
  throw std::out_of_range("No child at given index");
}

auto CommentNode::getChildCount() const -> unsigned int { return 0; }

auto CommentNode::getSpan() const -> input::Span { return m_comment.getSpan(); }

auto CommentNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto CommentNode::print(std::ostream& out) const -> std::ostream& {
  return out << "// " << m_comment;
}

// Factories.
auto commentNode(lex::Token comment) -> NodePtr {
  return std::unique_ptr<CommentNode>{new CommentNode{std::move(comment)}};
}

} // namespace parse
