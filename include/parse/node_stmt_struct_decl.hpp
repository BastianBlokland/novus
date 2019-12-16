#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/type.hpp"
#include "parse/type_substitution_list.hpp"
#include <utility>
#include <vector>

namespace parse {

class StructDeclStmtNode final : public Node {
public:
  class FieldSpec final {
  public:
    FieldSpec(Type type, lex::Token identifier);

    auto operator==(const FieldSpec& rhs) const noexcept -> bool;

    [[nodiscard]] auto getType() const noexcept -> const Type&;
    [[nodiscard]] auto getIdentifier() const noexcept -> const lex::Token&;

  private:
    Type m_type;
    lex::Token m_identifier;
  };

  friend auto structDeclStmtNode(
      lex::Token kw,
      lex::Token id,
      std::optional<TypeSubstitutionList> typeSubs,
      std::optional<lex::Token> eq,
      std::vector<StructDeclStmtNode::FieldSpec> fields,
      std::vector<lex::Token> commas) -> NodePtr;

  StructDeclStmtNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  [[nodiscard]] auto getId() const -> const lex::Token&;
  [[nodiscard]] auto getTypeSubs() const -> const std::optional<TypeSubstitutionList>&;
  [[nodiscard]] auto getFields() const -> const std::vector<FieldSpec>&;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const lex::Token m_kw;
  const lex::Token m_id;
  const std::optional<TypeSubstitutionList> m_typeSubs;
  const std::optional<lex::Token> m_eq;
  const std::vector<FieldSpec> m_fields;
  const std::vector<lex::Token> m_commas;

  StructDeclStmtNode(
      lex::Token kw,
      lex::Token id,
      std::optional<TypeSubstitutionList> typeSubs,
      std::optional<lex::Token> eq,
      std::vector<FieldSpec> fields,
      std::vector<lex::Token> commas);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto structDeclStmtNode(
    lex::Token kw,
    lex::Token id,
    std::optional<TypeSubstitutionList> typeSubs,
    std::optional<lex::Token> eq,
    std::vector<StructDeclStmtNode::FieldSpec> fields,
    std::vector<lex::Token> commas) -> NodePtr;

} // namespace parse
