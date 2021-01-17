#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/type_param_list.hpp"

namespace parse {

// Intrinsic expression node.
// Example in source: 'intrinsic{fileOpenStream}'.
class IntrinsicExprNode final : public Node {
  friend auto intrinsicExprNode(
      lex::Token kw,
      lex::Token open,
      lex::Token intrinsic,
      lex::Token close,
      std::optional<TypeParamList> typeParams) -> NodePtr;

public:
  IntrinsicExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  [[nodiscard]] auto getIntrinsic() const -> const lex::Token&;
  [[nodiscard]] auto getTypeParams() const -> const std::optional<TypeParamList>&;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const lex::Token m_kw;
  const lex::Token m_open;
  const lex::Token m_intrinsic;
  const lex::Token m_close;
  const std::optional<TypeParamList> m_typeParams;

  explicit IntrinsicExprNode(
      lex::Token kw,
      lex::Token open,
      lex::Token intrinsic,
      lex::Token close,
      std::optional<TypeParamList> typeParams);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto intrinsicExprNode(
    lex::Token kw,
    lex::Token open,
    lex::Token intrinsic,
    lex::Token close,
    std::optional<TypeParamList> typeParams) -> NodePtr;

} // namespace parse
