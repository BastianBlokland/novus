#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/type_param_list.hpp"

namespace parse {

// Identifier expression node.
// Example in source: 'user'.
class IdExprNode final : public Node {
  friend auto idExprNode(lex::Token id, std::optional<TypeParamList> typeParams) -> NodePtr;

public:
  IdExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  [[nodiscard]] auto isSelf() const -> bool;
  [[nodiscard]] auto getId() const -> const lex::Token&;
  [[nodiscard]] auto getTypeParams() const -> const std::optional<TypeParamList>&;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const lex::Token m_id;
  const std::optional<TypeParamList> m_typeParams;

  explicit IdExprNode(lex::Token id, std::optional<TypeParamList> typeParams);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto idExprNode(lex::Token id, std::optional<TypeParamList> typeParams) -> NodePtr;

} // namespace parse
