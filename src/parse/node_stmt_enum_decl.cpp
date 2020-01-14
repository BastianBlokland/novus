#include "parse/node_stmt_enum_decl.hpp"
#include "utilities.hpp"

namespace parse {

static auto getIdOrErr(const lex::Token& token) {
  return getId(token).value_or(std::string("err"));
}

EnumDeclStmtNode::ValueSpec::ValueSpec(lex::Token colon, lex::Token value) :
    m_colon{std::move(colon)}, m_value{std::move(value)} {}

auto EnumDeclStmtNode::ValueSpec::operator==(const ValueSpec& rhs) const noexcept -> bool {
  return m_colon == rhs.m_colon && m_value == rhs.m_value;
}

auto EnumDeclStmtNode::ValueSpec::getColon() const noexcept -> const lex::Token& { return m_colon; }

auto EnumDeclStmtNode::ValueSpec::getValue() const noexcept -> const lex::Token& { return m_value; }

auto EnumDeclStmtNode::ValueSpec::validate() const noexcept -> bool {
  return m_colon.getKind() == lex::TokenKind::SepColon &&
      m_value.getKind() == lex::TokenKind::LitInt;
}

EnumDeclStmtNode::EntrySpec::EntrySpec(lex::Token identifier, std::optional<ValueSpec> value) :
    m_identifier{std::move(identifier)}, m_value{std::move(value)} {}

auto EnumDeclStmtNode::EntrySpec::operator==(const EntrySpec& rhs) const noexcept -> bool {
  return m_identifier == rhs.m_identifier && m_value == rhs.m_value;
}

auto EnumDeclStmtNode::EntrySpec::getSpan() const -> input::Span {
  return m_value ? input::Span::combine(m_identifier.getSpan(), m_value->getValue().getSpan())
                 : m_identifier.getSpan();
}

auto EnumDeclStmtNode::EntrySpec::getIdentifier() const noexcept -> const lex::Token& {
  return m_identifier;
}

auto EnumDeclStmtNode::EntrySpec::getValueSpec() const noexcept -> const std::optional<ValueSpec>& {
  return m_value;
}

auto EnumDeclStmtNode::EntrySpec::validate() const noexcept -> bool {
  return m_identifier.getKind() == lex::TokenKind::Identifier &&
      (m_value == std::nullopt || m_value->validate());
}

EnumDeclStmtNode::EnumDeclStmtNode(
    lex::Token kw,
    lex::Token id,
    lex::Token eq,
    std::vector<EntrySpec> entries,
    std::vector<lex::Token> commas) :
    m_kw{std::move(kw)},
    m_id{std::move(id)},
    m_eq{std::move(eq)},
    m_entries{std::move(entries)},
    m_commas{std::move(commas)} {}

auto EnumDeclStmtNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const EnumDeclStmtNode*>(&rhs);
  return r != nullptr && m_id == r->m_id && m_entries == r->m_entries;
}

auto EnumDeclStmtNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !EnumDeclStmtNode::operator==(rhs);
}

auto EnumDeclStmtNode::operator[](unsigned int /*unused*/) const -> const Node& {
  throw std::out_of_range{"No child at given index"};
}

auto EnumDeclStmtNode::getChildCount() const -> unsigned int { return 0; }

auto EnumDeclStmtNode::getSpan() const -> input::Span {
  return input::Span::combine(m_kw.getSpan(), m_entries.back().getIdentifier().getSpan());
}

auto EnumDeclStmtNode::getId() const -> const lex::Token& { return m_id; }

auto EnumDeclStmtNode::getEntries() const -> const std::vector<EntrySpec>& { return m_entries; }

auto EnumDeclStmtNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto EnumDeclStmtNode::print(std::ostream& out) const -> std::ostream& {
  out << "enum-" << getIdOrErr(m_id) << '=';
  for (auto i = 0U; i < m_entries.size(); ++i) {
    if (i != 0) {
      out << ",";
    }
    const auto& entry = m_entries[i];
    out << getIdOrErr(entry.getIdentifier());
    if (entry.getValueSpec()) {
      out << ':' << entry.getValueSpec()->getValue();
    }
  }
  return out;
}

// Factories.
auto enumDeclStmtNode(
    lex::Token kw,
    lex::Token id,
    lex::Token eq,
    std::vector<EnumDeclStmtNode::EntrySpec> entries,
    std::vector<lex::Token> commas) -> NodePtr {
  if (entries.empty()) {
    throw std::invalid_argument{"Enum needs atleast one field"};
  }
  if (commas.size() != entries.size() - 1) {
    throw std::invalid_argument{"Incorrect number of commas"};
  }
  return std::unique_ptr<EnumDeclStmtNode>{new EnumDeclStmtNode{
      std::move(kw), std::move(id), std::move(eq), std::move(entries), std::move(commas)}};
}

} // namespace parse
