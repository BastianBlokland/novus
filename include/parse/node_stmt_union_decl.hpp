#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include <utility>
#include <vector>

namespace parse {

class UnionDeclStmtNode final : public Node {
  friend auto unionDeclStmtNode(
      lex::Token kw,
      lex::Token id,
      lex::Token eq,
      std::vector<lex::Token> types,
      std::vector<lex::Token> commas) -> NodePtr;

public:
  UnionDeclStmtNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  [[nodiscard]] auto getId() const -> const lex::Token&;
  [[nodiscard]] auto getTypes() const -> const std::vector<lex::Token>&;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const lex::Token m_kw;
  const lex::Token m_id;
  const lex::Token m_eq;
  const std::vector<lex::Token> m_types;
  const std::vector<lex::Token> m_commas;

  UnionDeclStmtNode(
      lex::Token kw,
      lex::Token id,
      lex::Token eq,
      std::vector<lex::Token> types,
      std::vector<lex::Token> commas);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto unionDeclStmtNode(
    lex::Token kw,
    lex::Token id,
    lex::Token eq,
    std::vector<lex::Token> types,
    std::vector<lex::Token> commas) -> NodePtr;

} // namespace parse
