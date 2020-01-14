#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/type.hpp"
#include <optional>
#include <utility>
#include <vector>

namespace parse {

class EnumDeclStmtNode final : public Node {
public:
  class ValueSpec final {
  public:
    ValueSpec(lex::Token colon, lex::Token value);

    auto operator==(const ValueSpec& rhs) const noexcept -> bool;

    [[nodiscard]] auto getColon() const noexcept -> const lex::Token&;
    [[nodiscard]] auto getValue() const noexcept -> const lex::Token&;

    [[nodiscard]] auto validate() const noexcept -> bool;

  private:
    lex::Token m_colon;
    lex::Token m_value;
  };

  class EntrySpec final {
  public:
    EntrySpec(lex::Token identifier, std::optional<ValueSpec> value);

    auto operator==(const EntrySpec& rhs) const noexcept -> bool;

    [[nodiscard]] auto getSpan() const -> input::Span;
    [[nodiscard]] auto getIdentifier() const noexcept -> const lex::Token&;
    [[nodiscard]] auto getValueSpec() const noexcept -> const std::optional<ValueSpec>&;

    [[nodiscard]] auto validate() const noexcept -> bool;

  private:
    lex::Token m_identifier;
    std::optional<ValueSpec> m_value;
  };

  friend auto enumDeclStmtNode(
      lex::Token kw,
      lex::Token id,
      lex::Token eq,
      std::vector<EnumDeclStmtNode::EntrySpec> entries,
      std::vector<lex::Token> commas) -> NodePtr;

  EnumDeclStmtNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  [[nodiscard]] auto getId() const -> const lex::Token&;
  [[nodiscard]] auto getEntries() const -> const std::vector<EntrySpec>&;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const lex::Token m_kw;
  const lex::Token m_id;
  const lex::Token m_eq;
  const std::vector<EntrySpec> m_entries;
  const std::vector<lex::Token> m_commas;

  EnumDeclStmtNode(
      lex::Token kw,
      lex::Token id,
      lex::Token eq,
      std::vector<EntrySpec> entries,
      std::vector<lex::Token> commas);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto enumDeclStmtNode(
    lex::Token kw,
    lex::Token id,
    lex::Token eq,
    std::vector<EnumDeclStmtNode::EntrySpec> entries,
    std::vector<lex::Token> commas) -> NodePtr;

} // namespace parse
