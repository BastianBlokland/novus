#include "parse/node_expr_id.hpp"
#include "lex/keyword.hpp"
#include "utilities.hpp"

namespace parse {

IdExprNode::IdExprNode(lex::Token id, std::optional<TypeParamList> typeParams) :
    m_id{std::move(id)}, m_typeParams{std::move(typeParams)} {}

auto IdExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const IdExprNode*>(&rhs);
  return r != nullptr && m_id == r->m_id && m_typeParams == r->m_typeParams;
}

auto IdExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !IdExprNode::operator==(rhs);
}

auto IdExprNode::operator[](unsigned int /*unused*/) const -> const Node& {
  throw std::out_of_range{"No child at given index"};
}

auto IdExprNode::getChildCount() const -> unsigned int { return 0; }

auto IdExprNode::getSpan() const -> input::Span {
  if (m_typeParams) {
    return input::Span::combine(m_id.getSpan(), m_typeParams->getSpan());
  }
  return m_id.getSpan();
}

auto IdExprNode::isSelf() const -> bool { return getKw(m_id) == lex::Keyword::Self; }

auto IdExprNode::getId() const -> const lex::Token& { return m_id; }

auto IdExprNode::getTypeParams() const -> const std::optional<TypeParamList>& {
  return m_typeParams;
}

auto IdExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto IdExprNode::print(std::ostream& out) const -> std::ostream& {
  out << (isSelf() ? "self" : ::parse::getId(m_id).value_or("error"));
  if (m_typeParams) {
    out << *m_typeParams;
  }
  return out;
}

// Factories.
auto idExprNode(lex::Token id, std::optional<TypeParamList> typeParams) -> NodePtr {
  return std::unique_ptr<IdExprNode>{new IdExprNode{std::move(id), std::move(typeParams)}};
}

} // namespace parse
