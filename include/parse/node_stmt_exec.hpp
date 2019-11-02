#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include <vector>

namespace parse {

class ExecStmtNode final : public Node {
  friend auto execStmtNode(
      lex::Token action,
      lex::Token open,
      std::vector<NodePtr> args,
      std::vector<lex::Token> commas,
      lex::Token close) -> NodePtr;

public:
  ExecStmtNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  [[nodiscard]] auto getAction() const -> const lex::Token&;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const lex::Token m_action;
  const lex::Token m_open;
  const std::vector<NodePtr> m_args;
  const std::vector<lex::Token> m_commas;
  const lex::Token m_close;

  ExecStmtNode(
      lex::Token action,
      lex::Token open,
      std::vector<NodePtr> args,
      std::vector<lex::Token> commas,
      lex::Token close);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto execStmtNode(
    lex::Token action,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) -> NodePtr;

} // namespace parse
