#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/type_param_list.hpp"
#include <utility>
#include <vector>

namespace parse {

class FuncDeclStmtNode final : public Node {
public:
  class ArgSpec final {
  public:
    ArgSpec(lex::Token type, lex::Token identifier);

    auto operator==(const ArgSpec& rhs) const noexcept -> bool;

    [[nodiscard]] auto getType() const noexcept -> const lex::Token&;
    [[nodiscard]] auto getIdentifier() const noexcept -> const lex::Token&;

  private:
    lex::Token m_type;
    lex::Token m_identifier;
  };

  class RetTypeSpec final {
  public:
    RetTypeSpec(lex::Token arrow, lex::Token type);

    auto operator==(const RetTypeSpec& rhs) const noexcept -> bool;

    [[nodiscard]] auto getArrow() const noexcept -> const lex::Token&;
    [[nodiscard]] auto getType() const noexcept -> const lex::Token&;

  private:
    lex::Token m_arrow;
    lex::Token m_type;
  };

  friend auto funcDeclStmtNode(
      lex::Token kw,
      lex::Token id,
      std::optional<TypeParamList> typeParams,
      lex::Token open,
      std::vector<ArgSpec> args,
      std::vector<lex::Token> commas,
      lex::Token close,
      std::optional<RetTypeSpec> retType,
      NodePtr body) -> NodePtr;

  FuncDeclStmtNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  [[nodiscard]] auto getId() const -> const lex::Token&;
  [[nodiscard]] auto getTypeParams() const -> const std::optional<TypeParamList>&;
  [[nodiscard]] auto getArgs() const -> const std::vector<ArgSpec>&;
  [[nodiscard]] auto getRetType() const -> const std::optional<RetTypeSpec>&;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const lex::Token m_kw;
  const lex::Token m_id;
  const std::optional<TypeParamList> m_typeParams;
  const lex::Token m_open;
  const std::vector<ArgSpec> m_args;
  const std::vector<lex::Token> m_commas;
  const lex::Token m_close;
  const std::optional<RetTypeSpec> m_retType;
  const NodePtr m_body;

  FuncDeclStmtNode(
      lex::Token kw,
      lex::Token id,
      std::optional<TypeParamList> typeParams,
      lex::Token open,
      std::vector<ArgSpec> args,
      std::vector<lex::Token> commas,
      lex::Token close,
      std::optional<RetTypeSpec> retType,
      NodePtr body);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto funcDeclStmtNode(
    lex::Token kw,
    lex::Token id,
    std::optional<TypeParamList> typeParams,
    lex::Token open,
    std::vector<FuncDeclStmtNode::ArgSpec> args,
    std::vector<lex::Token> commas,
    lex::Token close,
    std::optional<FuncDeclStmtNode::RetTypeSpec> retTypeSpec,
    NodePtr body) -> NodePtr;

} // namespace parse
