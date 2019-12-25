#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include <vector>

namespace parse {

class CommentNode final : public Node {
  friend auto commentNode(lex::Token comment) -> NodePtr;

public:
  CommentNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const lex::Token m_comment;

  explicit CommentNode(lex::Token comment);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto commentNode(lex::Token comment) -> NodePtr;

} // namespace parse
