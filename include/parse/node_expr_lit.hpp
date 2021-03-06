#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"

namespace parse {

// Literal expression.
// Example in source: '32' or '1.337' or '"hello world"'.
class LitExprNode final : public Node {
  friend auto litExprNode(lex::Token val) -> NodePtr;

public:
  LitExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  [[nodiscard]] auto getVal() const -> const lex::Token&;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const lex::Token m_val;

  explicit LitExprNode(lex::Token val);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto litExprNode(lex::Token val) -> NodePtr;

} // namespace parse
