#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/node_type.hpp"
#include <initializer_list>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace parse {

class ErrorNode final : public Node {
public:
  ErrorNode() = delete;

  ErrorNode(std::string msg, std::initializer_list<lex::Token> tokens) :
      Node(NodeType::Error), m_msg{std::move(msg)}, m_tokens{tokens} {}

  ErrorNode(std::string msg, std::vector<lex::Token> tokens) :
      Node(NodeType::Error), m_msg{std::move(msg)}, m_tokens{std::move(tokens)} {}

  auto operator==(const Node* rhs) const noexcept -> bool override {
    const auto castedRhs = dynamic_cast<const ErrorNode*>(rhs);
    return castedRhs != nullptr && m_msg == castedRhs->m_msg && m_tokens == castedRhs->m_tokens;
  }

  auto operator!=(const Node* rhs) const noexcept -> bool override {
    return !ErrorNode::operator==(rhs);
  }

  [[nodiscard]] auto clone() const -> std::unique_ptr<Node> override {
    return std::make_unique<ErrorNode>(m_msg, m_tokens);
  }

  [[nodiscard]] auto getMessage() const noexcept -> const std::string& { return m_msg; }

  [[nodiscard]] auto getTokens() const noexcept -> const std::vector<lex::Token>& {
    return m_tokens;
  }

private:
  const std::string m_msg;
  const std::vector<lex::Token> m_tokens;

  auto print(std::ostream& out) const -> std::ostream& override { return out << m_msg; }
};

// Factory.
template <typename... Tokens>
inline auto errorNode(std::string msg, Tokens... tokens) -> std::unique_ptr<Node> {
  return std::make_unique<ErrorNode>(std::move(msg), std::vector{tokens...});
}

} // namespace parse
