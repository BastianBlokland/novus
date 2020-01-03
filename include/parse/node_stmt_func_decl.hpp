#pragma once
#include "lex/token.hpp"
#include "parse/argument_list_decl.hpp"
#include "parse/node.hpp"
#include "parse/type_substitution_list.hpp"
#include <utility>
#include <vector>

namespace parse {

class FuncDeclStmtNode final : public Node {
public:
  class RetTypeSpec final {
  public:
    RetTypeSpec(lex::Token arrow, Type type);

    auto operator==(const RetTypeSpec& rhs) const noexcept -> bool;

    [[nodiscard]] auto getArrow() const noexcept -> const lex::Token&;
    [[nodiscard]] auto getType() const noexcept -> const Type&;

  private:
    lex::Token m_arrow;
    Type m_type;
  };

  friend auto funcDeclStmtNode(
      lex::Token kw,
      lex::Token id,
      std::optional<TypeSubstitutionList> typeSubs,
      ArgumentListDecl argList,
      std::optional<RetTypeSpec> retType,
      NodePtr body) -> NodePtr;

  FuncDeclStmtNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  [[nodiscard]] auto getId() const -> const lex::Token&;
  [[nodiscard]] auto getTypeSubs() const -> const std::optional<TypeSubstitutionList>&;
  [[nodiscard]] auto getArgList() const -> const ArgumentListDecl&;
  [[nodiscard]] auto getRetType() const -> const std::optional<RetTypeSpec>&;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const lex::Token m_kw;
  const lex::Token m_id;
  const std::optional<TypeSubstitutionList> m_typeSubs;
  const ArgumentListDecl m_argList;
  const std::optional<RetTypeSpec> m_retType;
  const NodePtr m_body;

  FuncDeclStmtNode(
      lex::Token kw,
      lex::Token id,
      std::optional<TypeSubstitutionList> typeSubs,
      ArgumentListDecl argList,
      std::optional<RetTypeSpec> retType,
      NodePtr body);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto funcDeclStmtNode(
    lex::Token kw,
    lex::Token id,
    std::optional<TypeSubstitutionList> typeSubs,
    ArgumentListDecl argList,
    std::optional<FuncDeclStmtNode::RetTypeSpec> retTypeSpec,
    NodePtr body) -> NodePtr;

} // namespace parse
