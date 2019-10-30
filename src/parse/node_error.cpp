#include "parse/node_error.hpp"
#include "utilities.hpp"
#include <algorithm>

namespace parse {

ErrorNode::ErrorNode(
    std::string msg, std::vector<lex::Token> tokens, std::vector<NodePtr> subExprs) :
    Node(NodeKind::Error),
    m_msg{std::move(msg)},
    m_tokens{std::move(tokens)},
    m_subExprs{std::move(subExprs)} {}

auto ErrorNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const ErrorNode*>(&rhs);
  return r != nullptr && m_msg == r->m_msg && m_tokens == r->m_tokens &&
      nodesEqual(m_subExprs, r->m_subExprs);
}

auto ErrorNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !ErrorNode::operator==(rhs);
}

auto ErrorNode::operator[](int i) const -> const Node& {
  if (i < 0 || static_cast<unsigned>(i) >= m_subExprs.size()) {
    throw std::out_of_range("No child at given index");
  }
  return *m_subExprs[i];
}

auto ErrorNode::getChildCount() const -> unsigned int { return m_subExprs.size(); }

auto ErrorNode::getSpan() const -> input::SourceSpan {
  /* Because there is no fixed order in a ErrorNode we need to find the start and
  the end source position by looking at all tokens and nodes. */
  auto tokensSpan   = ::parse::getSpan(m_tokens);
  auto subExprsSpan = ::parse::getSpan(m_subExprs);
  if (tokensSpan && subExprsSpan) {
    return input::SourceSpan::combine(*tokensSpan, *subExprsSpan);
  }
  return tokensSpan ? *tokensSpan : *subExprsSpan;
}

auto ErrorNode::getMessage() const noexcept -> const std::string& { return m_msg; }

auto ErrorNode::print(std::ostream& out) const -> std::ostream& { return out << m_msg; }

// Factories.
auto errorNode(std::string msg, std::vector<lex::Token> tokens, std::vector<NodePtr> subExprs)
    -> NodePtr {
  if (anyNodeNull(subExprs)) {
    throw std::invalid_argument("subExprs cannot contain a nullptr");
  }
  return std::unique_ptr<ErrorNode>{
      new ErrorNode{std::move(msg), std::move(tokens), std::move(subExprs)}};
}

auto errorNode(std::string msg, lex::Token token) -> NodePtr {
  auto tokens = std::vector<lex::Token>{};
  tokens.push_back(std::move(token));

  return errorNode(std::move(msg), std::move(tokens));
}

auto errorNode(std::string msg, lex::Token token, NodePtr subExpr) -> NodePtr {
  auto tokens = std::vector<lex::Token>{};
  tokens.push_back(std::move(token));

  auto subExprs = std::vector<NodePtr>{};
  subExprs.push_back((std::move(subExpr)));

  return errorNode(std::move(msg), std::move(tokens), std::move(subExprs));
}

} // namespace parse
