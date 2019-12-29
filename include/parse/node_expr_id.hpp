#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"

namespace parse {

class IdExprNode final : public Node {
  friend auto idExprNode(lex::Token id) -> NodePtr;

public:
  IdExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  [[nodiscard]] auto getId() const -> const lex::Token&;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const lex::Token m_id;

  explicit IdExprNode(lex::Token id);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto idExprNode(lex::Token id) -> NodePtr;

} // namespace parse
