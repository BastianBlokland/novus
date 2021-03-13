#include "parse/node_expr_anon_func.hpp"
#include "utilities.hpp"

namespace parse {

AnonFuncExprNode::AnonFuncExprNode(
    std::vector<lex::Token> modifiers,
    lex::Token kw,
    ArgumentListDecl argList,
    std::optional<RetTypeSpec> retType,
    NodePtr body) :
    m_modifiers{std::move(modifiers)},
    m_kw{std::move(kw)},
    m_argList{std::move(argList)},
    m_retType{std::move(retType)},
    m_body{std::move(body)} {}

auto AnonFuncExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const AnonFuncExprNode*>(&rhs);
  return r != nullptr && m_modifiers == r->m_modifiers && m_argList == r->m_argList &&
      m_retType == r->m_retType && *m_body == *r->m_body;
}

auto AnonFuncExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !AnonFuncExprNode::operator==(rhs);
}

auto AnonFuncExprNode::operator[](unsigned int i) const -> const Node& {
  const auto initializerCount = m_argList.getInitializerCount();
  if (i < initializerCount) {
    return m_argList.getInitializer(i);
  }
  if (i == initializerCount) {
    return *m_body;
  }
  throw std::out_of_range{"No child at given index"};
}

auto AnonFuncExprNode::getChildCount() const -> unsigned int {
  return m_argList.getInitializerCount() + 1;
}

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

auto AnonFuncExprNode::hasModifier(lex::Keyword /*unused*/) const -> bool { return false; }

auto AnonFuncExprNode::getArgList() const -> const ArgumentListDecl& { return m_argList; }

auto AnonFuncExprNode::getRetType() const -> const std::optional<RetTypeSpec>& { return m_retType; }

auto AnonFuncExprNode::getBody() const -> const Node& { return *m_body; }

auto AnonFuncExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto AnonFuncExprNode::print(std::ostream& out) const -> std::ostream& {
  for (const auto& mod : m_modifiers) {
    auto modKw = getKw(mod);
    if (modKw) {
      out << *modKw << '-';
    }
  }
  out << "anonfun-" << m_argList;
  if (m_retType) {
    out << *m_retType;
  }
  return out;
}

// Factories.
auto anonFuncExprNode(
    std::vector<lex::Token> modifiers,
    lex::Token kw,
    ArgumentListDecl argList,
    std::optional<RetTypeSpec> retType,
    NodePtr body) -> NodePtr {
  if (body == nullptr) {
    throw std::invalid_argument{"Body cannot be null"};
  }
  return std::unique_ptr<AnonFuncExprNode>{new AnonFuncExprNode{
      std::move(modifiers),
      std::move(kw),
      std::move(argList),
      std::move(retType),
      std::move(body)}};
}

} // namespace parse
