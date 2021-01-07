#include "parse/node_expr_intrinsic.hpp"
#include "lex/keyword.hpp"
#include "utilities.hpp"

namespace parse {

IntrinsicExprNode::IntrinsicExprNode(
    lex::Token kw, lex::Token open, lex::Token intrinsic, lex::Token close) :
    m_kw{std::move(kw)},
    m_open{std::move(open)},
    m_intrinsic{std::move(intrinsic)},
    m_close{std::move(close)} {}

auto IntrinsicExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const IntrinsicExprNode*>(&rhs);
  return r != nullptr && m_kw == r->m_kw && m_intrinsic == r->m_intrinsic;
}

auto IntrinsicExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !IntrinsicExprNode::operator==(rhs);
}

auto IntrinsicExprNode::operator[](unsigned int /*unused*/) const -> const Node& {
  throw std::out_of_range{"No child at given index"};
}

auto IntrinsicExprNode::getChildCount() const -> unsigned int { return 0; }

auto IntrinsicExprNode::getSpan() const -> input::Span {
  return input::Span::combine(m_kw.getSpan(), m_close.getSpan());
}

auto IntrinsicExprNode::getIntrinsic() const -> const lex::Token& { return m_intrinsic; }

auto IntrinsicExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto IntrinsicExprNode::print(std::ostream& out) const -> std::ostream& {
  out << "intrinsic{" << parse::getId(m_intrinsic).value_or("error") << "}";
  return out;
}

// Factories.
auto intrinsicExprNode(lex::Token kw, lex::Token open, lex::Token intrinsic, lex::Token close)
    -> NodePtr {
  return std::unique_ptr<IntrinsicExprNode>{new IntrinsicExprNode{
      std::move(kw), std::move(open), std::move(intrinsic), std::move(close)}};
}

} // namespace parse
