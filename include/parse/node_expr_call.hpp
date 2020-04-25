#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include <vector>

namespace parse {

class CallExprNode final : public Node {
  friend auto callExprNode(
      std::vector<lex::Token> modifiers,
      NodePtr lhs,
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

  [[nodiscard]] auto isFork() const -> bool;
  [[nodiscard]] auto isLazy() const -> bool;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const std::vector<lex::Token> m_modifiers;
  const NodePtr m_lhs;
  const lex::Token m_open;
  const std::vector<NodePtr> m_args;
  const std::vector<lex::Token> m_commas;
  const lex::Token m_close;

  CallExprNode(
      std::vector<lex::Token> modifiers,
      NodePtr lhs,
      lex::Token open,
      std::vector<NodePtr> args,
      std::vector<lex::Token> commas,
      lex::Token close);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto callExprNode(
    std::vector<lex::Token> modifiers,
    NodePtr lhs,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) -> NodePtr;

} // namespace parse
