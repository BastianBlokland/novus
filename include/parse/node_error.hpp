#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include <vector>

namespace parse {

// Node containing a parse error.
// Note: Contains all tokens and sub expression that where part of the error.
class ErrorNode final : public Node {
  friend auto
  errorNode(std::string msg, std::vector<lex::Token> tokens, std::vector<NodePtr> subExprs)
      -> NodePtr;

public:
  ErrorNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  [[nodiscard]] auto getMessage() const noexcept -> const std::string&;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const std::string m_msg;
  const std::vector<lex::Token> m_tokens;
  const std::vector<NodePtr> m_subExprs;

  ErrorNode(std::string msg, std::vector<lex::Token> tokens, std::vector<NodePtr> subExprs);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto errorNode(
    std::string msg, std::vector<lex::Token> tokens = {}, std::vector<NodePtr> subExprs = {})
    -> NodePtr;

auto errorNode(std::string msg, lex::Token token) -> NodePtr;

auto errorNode(std::string msg, lex::Token token, NodePtr subExpr) -> NodePtr;

} // namespace parse
