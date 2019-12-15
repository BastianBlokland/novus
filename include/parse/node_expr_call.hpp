#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/type_param_list.hpp"
#include <vector>

namespace parse {

class CallExprNode final : public Node {
  friend auto callExprNode(
      lex::Token func,
      std::optional<TypeParamList> typeParams,
      lex::Token open,
      std::vector<NodePtr> args,
      std::vector<lex::Token> commas,
      lex::Token close) -> NodePtr;

public:
  CallExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  [[nodiscard]] auto getFunc() const -> const lex::Token&;
  [[nodiscard]] auto getTypeParams() const -> const std::optional<TypeParamList>&;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const lex::Token m_func;
  const std::optional<TypeParamList> m_typeParams;
  const lex::Token m_open;
  const std::vector<NodePtr> m_args;
  const std::vector<lex::Token> m_commas;
  const lex::Token m_close;

  CallExprNode(
      lex::Token func,
      std::optional<TypeParamList> typeParams,
      lex::Token open,
      std::vector<NodePtr> args,
      std::vector<lex::Token> commas,
      lex::Token close);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto callExprNode(
    lex::Token func,
    std::optional<TypeParamList> typeParams,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) -> NodePtr;

} // namespace parse
