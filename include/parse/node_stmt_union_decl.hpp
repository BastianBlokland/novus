#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/type.hpp"
#include "parse/type_substitution_list.hpp"
#include <utility>
#include <vector>

namespace parse {

class UnionDeclStmtNode final : public Node {
  friend auto unionDeclStmtNode(
      lex::Token kw,
      lex::Token id,
      std::optional<TypeSubstitutionList> typeSubs,
      lex::Token eq,
      std::vector<Type> types,
      std::vector<lex::Token> commas) -> NodePtr;

public:
  UnionDeclStmtNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  [[nodiscard]] auto getId() const -> const lex::Token&;
  [[nodiscard]] auto getTypeSubs() const -> const std::optional<TypeSubstitutionList>&;
  [[nodiscard]] auto getTypes() const -> const std::vector<Type>&;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const lex::Token m_kw;
  const lex::Token m_id;
  const std::optional<TypeSubstitutionList> m_typeSubs;
  const lex::Token m_eq;
  const std::vector<Type> m_types;
  const std::vector<lex::Token> m_commas;

  UnionDeclStmtNode(
      lex::Token kw,
      lex::Token id,
      std::optional<TypeSubstitutionList> typeSubs,
      lex::Token eq,
      std::vector<Type> types,
      std::vector<lex::Token> commas);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto unionDeclStmtNode(
    lex::Token kw,
    lex::Token id,
    std::optional<TypeSubstitutionList> typeSubs,
    lex::Token eq,
    std::vector<Type> types,
    std::vector<lex::Token> commas) -> NodePtr;

} // namespace parse
