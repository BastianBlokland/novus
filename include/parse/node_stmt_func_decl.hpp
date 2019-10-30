#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include <vector>

namespace parse {

class FuncDeclStmtNode final : public Node {
  friend auto funcDeclStmtNode(
      lex::Token kw,
      lex::Token id,
      lex::Token open,
      std::vector<std::pair<lex::Token, lex::Token>> args,
      std::vector<lex::Token> commas,
      lex::Token close,
      lex::Token arrow,
      lex::Token retType,
      NodePtr body) -> NodePtr;

public:
  using arg = typename std::pair<lex::Token, lex::Token>;

  FuncDeclStmtNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  [[nodiscard]] auto getId() const -> const lex::Token&;
  [[nodiscard]] auto getArgs() const -> const std::vector<arg>&;
  [[nodiscard]] auto getRetType() const -> const lex::Token&;

private:
  const lex::Token m_kw;
  const lex::Token m_id;
  const lex::Token m_open;
  const std::vector<arg> m_args;
  const std::vector<lex::Token> m_commas;
  const lex::Token m_close;
  const lex::Token m_arrow;
  const lex::Token m_retType;
  const NodePtr m_body;

  FuncDeclStmtNode(
      lex::Token kw,
      lex::Token id,
      lex::Token open,
      std::vector<arg> args,
      std::vector<lex::Token> commas,
      lex::Token close,
      lex::Token arrow,
      lex::Token retType,
      NodePtr body);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto funcDeclStmtNode(
    lex::Token kw,
    lex::Token id,
    lex::Token open,
    std::vector<FuncDeclStmtNode::arg> args,
    std::vector<lex::Token> commas,
    lex::Token close,
    lex::Token arrow,
    lex::Token retType,
    NodePtr body) -> NodePtr;

} // namespace parse
