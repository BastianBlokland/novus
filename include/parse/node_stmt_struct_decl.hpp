#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include <utility>
#include <vector>

namespace parse {

class StructDeclStmtNode final : public Node {
public:
  class FieldSpec final {
  public:
    FieldSpec(lex::Token type, lex::Token identifier);

    auto operator==(const FieldSpec& rhs) const noexcept -> bool;

    [[nodiscard]] auto getType() const noexcept -> const lex::Token&;
    [[nodiscard]] auto getIdentifier() const noexcept -> const lex::Token&;

  private:
    lex::Token m_type;
    lex::Token m_identifier;
  };

  friend auto structDeclStmtNode(
      lex::Token kw,
      lex::Token id,
      lex::Token eq,
      std::vector<StructDeclStmtNode::FieldSpec> fields,
      std::vector<lex::Token> commas) -> NodePtr;

  StructDeclStmtNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  [[nodiscard]] auto getId() const -> const lex::Token&;
  [[nodiscard]] auto getFields() const -> const std::vector<FieldSpec>&;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const lex::Token m_kw;
  const lex::Token m_id;
  const lex::Token m_eq;
  const std::vector<FieldSpec> m_fields;
  const std::vector<lex::Token> m_commas;

  StructDeclStmtNode(
      lex::Token kw,
      lex::Token id,
      lex::Token eq,
      std::vector<FieldSpec> fields,
      std::vector<lex::Token> commas);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto structDeclStmtNode(
    lex::Token kw,
    lex::Token id,
    lex::Token eq,
    std::vector<StructDeclStmtNode::FieldSpec> fields,
    std::vector<lex::Token> commas) -> NodePtr;

} // namespace parse
