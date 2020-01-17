#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/type.hpp"
#include <optional>

namespace parse {

class IsExprNode final : public Node {
  friend auto isExprNode(NodePtr lhs, lex::Token kw, Type type, std::optional<lex::Token> id)
      -> NodePtr;

public:
  IsExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  [[nodiscard]] auto getType() const -> const Type&;
  [[nodiscard]] auto hasId() const -> bool;
  [[nodiscard]] auto getId() const -> const std::optional<lex::Token>&;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const NodePtr m_lhs;
  const lex::Token m_kw;
  const Type m_type;
  const std::optional<lex::Token> m_id;

  IsExprNode(NodePtr lhs, lex::Token kw, Type type, std::optional<lex::Token> id);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto isExprNode(NodePtr lhs, lex::Token kw, Type type, std::optional<lex::Token> id) -> NodePtr;

} // namespace parse
