#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"

namespace parse {

class LitExprNode final : public Node {
public:
  LitExprNode() = delete;
  explicit LitExprNode(lex::Token val);

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;

private:
  const lex::Token m_val;

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto litExprNode(lex::Token val) -> NodePtr;

} // namespace parse
