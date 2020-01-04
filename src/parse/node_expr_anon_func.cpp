#include "parse/node_expr_anon_func.hpp"
#include "utilities.hpp"

namespace parse {

AnonFuncExprNode::AnonFuncExprNode(lex::Token kw, ArgumentListDecl argList, NodePtr body) :
    m_kw{std::move(kw)}, m_argList{std::move(argList)}, m_body{std::move(body)} {}

auto AnonFuncExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const AnonFuncExprNode*>(&rhs);
  return r != nullptr && m_argList == r->m_argList && *m_body == *r->m_body;
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
  return input::Span::combine(m_kw.getSpan(), m_body->getSpan());
}

auto AnonFuncExprNode::getArgList() const -> const ArgumentListDecl& { return m_argList; }

auto AnonFuncExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto AnonFuncExprNode::print(std::ostream& out) const -> std::ostream& {
  out << "anonfun-";
  out << m_argList;
  return out;
}

// Factories.
auto anonFuncExprNode(lex::Token kw, ArgumentListDecl argList, NodePtr body) -> NodePtr {
  if (body == nullptr) {
    throw std::invalid_argument{"Body cannot be null"};
  }
  return std::unique_ptr<AnonFuncExprNode>{
      new AnonFuncExprNode{std::move(kw), std::move(argList), std::move(body)}};
}

} // namespace parse
