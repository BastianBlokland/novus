#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/node_type.hpp"
#include <vector>

namespace parse {

class ErrorNode final : public Node {
public:
  ErrorNode() = delete;
  ErrorNode(std::string msg, std::vector<lex::Token> tokens, std::vector<NodePtr> subExprs);

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getMessage() const noexcept -> const std::string&;

private:
  const std::string m_msg;
  const std::vector<lex::Token> m_tokens;
  const std::vector<NodePtr> m_subExprs;

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto errorNode(
    std::string msg, std::vector<lex::Token> tokens = {}, std::vector<NodePtr> subExprs = {})
    -> NodePtr;

auto errorNode(std::string msg, lex::Token token) -> NodePtr;

auto errorNode(std::string msg, lex::Token token, NodePtr subExpr) -> NodePtr;

} // namespace parse
