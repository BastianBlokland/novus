#include "parse/node_expr_is.hpp"
#include "utilities.hpp"

namespace parse {

IsExprNode::IsExprNode(NodePtr lhs, lex::Token kw, Type type, std::optional<lex::Token> id) :
    m_lhs{std::move(lhs)}, m_kw{std::move(kw)}, m_type{std::move(type)}, m_id{std::move(id)} {}

auto IsExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const IsExprNode*>(&rhs);
  return r != nullptr && *m_lhs == *r->m_lhs && m_kw == r->m_kw && m_type == r->m_type &&
      m_id == r->m_id;
}

auto IsExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !IsExprNode::operator==(rhs);
}

auto IsExprNode::operator[](unsigned int i) const -> const Node& {
  if (i == 0) {
    return *m_lhs;
  }
  throw std::out_of_range{"No child at given index"};
}

auto IsExprNode::getChildCount() const -> unsigned int { return 1; }

auto IsExprNode::getSpan() const -> input::Span {
  if (m_id) {
    return input::Span::combine(m_lhs->getSpan(), m_id->getSpan());
  }
  return input::Span::combine(m_lhs->getSpan(), m_type.getSpan());
}

auto IsExprNode::getType() const -> const Type& { return m_type; }

auto IsExprNode::hasId() const -> bool {
  return m_id && m_id->getKind() == lex::TokenKind::Identifier;
}

auto IsExprNode::getId() const -> const std::optional<lex::Token>& { return m_id; }

auto IsExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto IsExprNode::print(std::ostream& out) const -> std::ostream& {
  out << m_kw << '-' << m_type;
  if (hasId()) {
    out << '-' << parse::getId(*m_id).value_or("error");
  }
  return out;
}

// Factories.
auto isExprNode(NodePtr lhs, lex::Token kw, Type type, std::optional<lex::Token> id) -> NodePtr {
  if (lhs == nullptr) {
    throw std::invalid_argument{"Lhs cannot be null"};
  }
  return std::unique_ptr<IsExprNode>{
      new IsExprNode{std::move(lhs), std::move(kw), std::move(type), std::move(id)}};
}

} // namespace parse
