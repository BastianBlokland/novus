#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/node_type.hpp"
#include "parse/utilities.hpp"
#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace parse {

class ErrorNode final : public Node {
public:
  ErrorNode() = delete;

  ErrorNode(std::string msg, std::vector<lex::Token> tokens, std::vector<NodePtr> subExprs) :
      Node(NodeType::Error),
      m_msg{std::move(msg)},
      m_tokens{std::move(tokens)},
      m_subExprs{std::move(subExprs)} {}

  auto operator==(const Node& rhs) const noexcept -> bool override {
    const auto r = dynamic_cast<const ErrorNode*>(&rhs);
    return r != nullptr && m_msg == r->m_msg && m_tokens == r->m_tokens &&
        std::equal(
               m_subExprs.begin(),
               m_subExprs.end(),
               r->m_subExprs.begin(),
               [](const NodePtr& l, const NodePtr& r) { return *l == *r; });
  }

  auto operator!=(const Node& rhs) const noexcept -> bool override {
    return !ErrorNode::operator==(rhs);
  }

  [[nodiscard]] auto operator[](int i) const -> const Node& override {
    if (i < 0 || static_cast<unsigned>(i) >= m_subExprs.size()) {
      throw std::out_of_range("No child at given index");
    }
    return *m_subExprs[i];
  }

  [[nodiscard]] auto getChildCount() const -> int override { return m_subExprs.size(); }

  [[nodiscard]] auto clone() const -> NodePtr override {
    return std::make_unique<ErrorNode>(m_msg, m_tokens, cloneSet(m_subExprs));
  }

  [[nodiscard]] auto getMessage() const noexcept -> const std::string& { return m_msg; }

  [[nodiscard]] auto getTokens() const noexcept -> const std::vector<lex::Token>& {
    return m_tokens;
  }

private:
  const std::string m_msg;
  const std::vector<lex::Token> m_tokens;
  const std::vector<NodePtr> m_subExprs;

  auto print(std::ostream& out) const -> std::ostream& override { return out << m_msg; }
};

// Factories.
inline auto
errorNode(std::string msg, std::vector<lex::Token> tokens = {}, std::vector<NodePtr> subExprs = {})
    -> NodePtr {
  return std::make_unique<ErrorNode>(std::move(msg), std::move(tokens), std::move(subExprs));
}

inline auto errorNode(std::string msg, lex::Token token) -> NodePtr {
  auto tokens = std::vector<lex::Token>{};
  tokens.push_back(std::move(token));

  return errorNode(std::move(msg), std::move(tokens));
}

inline auto errorNode(std::string msg, lex::Token token, NodePtr subExpr) -> NodePtr {
  auto tokens = std::vector<lex::Token>{};
  tokens.push_back(std::move(token));

  auto subExprs = std::vector<NodePtr>{};
  subExprs.push_back((std::move(subExpr)));

  return errorNode(std::move(msg), std::move(tokens), std::move(subExprs));
}

} // namespace parse
