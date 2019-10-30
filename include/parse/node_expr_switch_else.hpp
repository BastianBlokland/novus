#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"

namespace parse {

class SwitchExprElseNode final : public Node {
  friend auto switchExprElseNode(lex::Token kw, lex::Token arrow, NodePtr rhs) -> NodePtr;

public:
  SwitchExprElseNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

private:
  const lex::Token m_kw;
  const lex::Token m_arrow;
  const NodePtr m_rhs;

  SwitchExprElseNode(lex::Token kw, lex::Token arrow, NodePtr rhs);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto switchExprElseNode(lex::Token kw, lex::Token arrow, NodePtr rhs) -> NodePtr;

} // namespace parse
