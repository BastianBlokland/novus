#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"

namespace parse {

class SwitchExprIfNode final : public Node {
public:
  SwitchExprIfNode() = delete;
  SwitchExprIfNode(lex::Token kw, NodePtr cond, lex::Token arrow, NodePtr rhs);

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> lex::SourceSpan override;

private:
  const lex::Token m_kw;
  const NodePtr m_cond;
  const lex::Token m_arrow;
  const NodePtr m_rhs;

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto switchExprIfNode(lex::Token kw, NodePtr cond, lex::Token arrow, NodePtr rhs) -> NodePtr;

} // namespace parse
