#include "parse/node_expr_call.hpp"
#include "utilities.hpp"

namespace parse {

CallExprNode::CallExprNode(
    std::vector<lex::Token> modifiers,
    NodePtr lhs,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) :
    m_modifiers{std::move(modifiers)},
    m_lhs{std::move(lhs)},
    m_open{std::move(open)},
    m_args{std::move(args)},
    m_commas{std::move(commas)},
    m_close{std::move(close)} {}

auto CallExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const CallExprNode*>(&rhs);
  return r != nullptr && *m_lhs == *r->m_lhs && nodesEqual(m_args, r->m_args);
}

auto CallExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !CallExprNode::operator==(rhs);
}

auto CallExprNode::operator[](unsigned int i) const -> const Node& {
  if (i == 0) {
    return *m_lhs;
  }
  if (i > m_args.size()) {
    throw std::out_of_range{"No child at given index"};
  }
  return *m_args[i - 1];
}

auto CallExprNode::getChildCount() const -> unsigned int { return m_args.size() + 1; }

auto CallExprNode::getSpan() const -> input::Span {
  if (!m_modifiers.empty()) {
    return input::Span::combine(m_modifiers.begin()->getSpan(), m_close.getSpan());
  }
  return input::Span::combine(m_lhs->getSpan(), m_close.getSpan());
}

auto CallExprNode::isFork() const -> bool {
  return std::any_of(m_modifiers.begin(), m_modifiers.end(), [](const lex::Token& t) {
    return getKw(t) == lex::Keyword::Fork;
  });
}

auto CallExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto CallExprNode::print(std::ostream& out) const -> std::ostream& {
  for (const auto& mod : m_modifiers) {
    auto modKw = getKw(mod);
    if (modKw) {
      out << *modKw << '-';
    }
  }
  return out << "call";
}

// Factories.
auto callExprNode(
    std::vector<lex::Token> modifiers,
    NodePtr lhs,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) -> NodePtr {
  if (anyNodeNull(args)) {
    throw std::invalid_argument{"args cannot contain a nullptr"};
  }
  if (args.empty() ? !commas.empty() : commas.size() != args.size() - 1) {
    throw std::invalid_argument{"Incorrect number of commas"};
  }
  return std::unique_ptr<CallExprNode>{new CallExprNode{std::move(modifiers),
                                                        std::move(lhs),
                                                        std::move(open),
                                                        std::move(args),
                                                        std::move(commas),
                                                        std::move(close)}};
}

} // namespace parse
