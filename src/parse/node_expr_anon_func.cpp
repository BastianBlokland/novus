#include "parse/node_expr_anon_func.hpp"
#include "utilities.hpp"

namespace parse {

AnonFuncExprNode::AnonFuncExprNode(
    std::vector<lex::Token> modifiers, lex::Token kw, ArgumentListDecl argList, NodePtr body) :
    m_modifiers{std::move(modifiers)},
    m_kw{std::move(kw)},
    m_argList{std::move(argList)},
    m_body{std::move(body)} {}

auto AnonFuncExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const AnonFuncExprNode*>(&rhs);
  return r != nullptr && m_modifiers == r->m_modifiers && m_argList == r->m_argList &&
      *m_body == *r->m_body;
}

auto AnonFuncExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !AnonFuncExprNode::operator==(rhs);
}

auto AnonFuncExprNode::operator[](unsigned int i) const -> const Node& {
  if (i == 0) {
    return *m_body;
  }
  throw std::out_of_range{"No child at given index"};
}

auto AnonFuncExprNode::getChildCount() const -> unsigned int { return 1; }

auto AnonFuncExprNode::getSpan() const -> input::Span {
  if (!m_modifiers.empty()) {
    return input::Span::combine(m_modifiers.begin()->getSpan(), m_body->getSpan());
  }
  return input::Span::combine(m_kw.getSpan(), m_body->getSpan());
}

auto AnonFuncExprNode::isImpure() const -> bool {
  return std::any_of(m_modifiers.begin(), m_modifiers.end(), [](const lex::Token& t) {
    return getKw(t) == lex::Keyword::Impure;
  });
}

auto AnonFuncExprNode::getArgList() const -> const ArgumentListDecl& { return m_argList; }

auto AnonFuncExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto AnonFuncExprNode::print(std::ostream& out) const -> std::ostream& {
  for (const auto& mod : m_modifiers) {
    auto modKw = getKw(mod);
    if (modKw) {
      out << *modKw << '-';
    }
  }
  return out << "anonfun-" << m_argList;
  ;
}

// Factories.
auto anonFuncExprNode(
    std::vector<lex::Token> modifiers, lex::Token kw, ArgumentListDecl argList, NodePtr body)
    -> NodePtr {
  if (body == nullptr) {
    throw std::invalid_argument{"Body cannot be null"};
  }
  return std::unique_ptr<AnonFuncExprNode>{new AnonFuncExprNode{
      std::move(modifiers), std::move(kw), std::move(argList), std::move(body)}};
}

} // namespace parse
