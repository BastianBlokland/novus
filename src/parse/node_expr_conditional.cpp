#include "parse/node_expr_conditional.hpp"
#include "utilities.hpp"

namespace parse {

ConditionalExprNode::ConditionalExprNode(
    NodePtr cond, lex::Token qmark, NodePtr ifBranch, lex::Token colon, NodePtr elseBranch) :
    m_cond{std::move(cond)},
    m_qmark{std::move(qmark)},
    m_ifBranch{std::move(ifBranch)},
    m_colon{std::move(colon)},
    m_elseBranch{std::move(elseBranch)} {}

auto ConditionalExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const ConditionalExprNode*>(&rhs);
  return r != nullptr && *m_cond == *r->m_cond && m_qmark == r->m_qmark &&
      *m_ifBranch == *r->m_ifBranch && m_colon == r->m_colon && *m_elseBranch == *r->m_elseBranch;
}

auto ConditionalExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !ConditionalExprNode::operator==(rhs);
}

auto ConditionalExprNode::operator[](unsigned int i) const -> const Node& {
  switch (i) {
  case 0:
    return *m_cond;
  case 1:
    return *m_ifBranch;
  case 2:
    return *m_elseBranch;
  default:
    throw std::out_of_range{"No child at given index"};
  }
}

auto ConditionalExprNode::getChildCount() const -> unsigned int { return 3; }

auto ConditionalExprNode::getSpan() const -> input::Span {
  return input::Span::combine(m_cond->getSpan(), m_elseBranch->getSpan());
}

auto ConditionalExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto ConditionalExprNode::print(std::ostream& out) const -> std::ostream& {
  return out << "conditional";
}

// Factories.
auto conditionalExprNode(
    NodePtr cond, lex::Token qmark, NodePtr ifBranch, lex::Token colon, NodePtr elseBranch)
    -> NodePtr {
  if (cond == nullptr) {
    throw std::invalid_argument{"Condition expr cannot be null"};
  }
  if (ifBranch == nullptr) {
    throw std::invalid_argument{"If-branch cannot be null"};
  }
  if (elseBranch == nullptr) {
    throw std::invalid_argument{"Else-branch cannot be null"};
  }
  return std::unique_ptr<ConditionalExprNode>{new ConditionalExprNode{std::move(cond),
                                                                      std::move(qmark),
                                                                      std::move(ifBranch),
                                                                      std::move(colon),
                                                                      std::move(elseBranch)}};
}

} // namespace parse
