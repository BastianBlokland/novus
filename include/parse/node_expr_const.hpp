#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"

namespace parse {

class ConstExprNode final : public Node {
public:
  ConstExprNode() = delete;
  explicit ConstExprNode(lex::Token id);

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> lex::SourceSpan override;

  [[nodiscard]] auto getId() const -> const lex::Token&;

private:
  const lex::Token m_id;

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto constExprNode(lex::Token id) -> NodePtr;

} // namespace parse
