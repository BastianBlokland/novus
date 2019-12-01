#include "parse/node_stmt_struct_decl.hpp"
#include "utilities.hpp"

namespace parse {

static auto getIdOrErr(const lex::Token& token) {
  return getId(token).value_or(std::string("err"));
}

StructDeclStmtNode::FieldSpec::FieldSpec(lex::Token type, lex::Token identifier) :
    m_type{std::move(type)}, m_identifier{std::move(identifier)} {}

auto StructDeclStmtNode::FieldSpec::operator==(const FieldSpec& rhs) const noexcept -> bool {
  return m_type == rhs.m_type && m_identifier == rhs.m_identifier;
}

auto StructDeclStmtNode::FieldSpec::getType() const noexcept -> const lex::Token& { return m_type; }

auto StructDeclStmtNode::FieldSpec::getIdentifier() const noexcept -> const lex::Token& {
  return m_identifier;
}

StructDeclStmtNode::StructDeclStmtNode(
    lex::Token kw,
    lex::Token id,
    lex::Token eq,
    std::vector<StructDeclStmtNode::FieldSpec> fields,
    std::vector<lex::Token> commas) :
    m_kw{std::move(kw)},
    m_id{std::move(id)},
    m_eq{std::move(eq)},
    m_fields{std::move(fields)},
    m_commas{std::move(commas)} {}

auto StructDeclStmtNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const StructDeclStmtNode*>(&rhs);
  return r != nullptr && m_id == r->m_id && m_fields == r->m_fields;
}

auto StructDeclStmtNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !StructDeclStmtNode::operator==(rhs);
}

auto StructDeclStmtNode::operator[](unsigned int /*unused*/) const -> const Node& {
  throw std::out_of_range{"No child at given index"};
}

auto StructDeclStmtNode::getChildCount() const -> unsigned int { return 0; }

auto StructDeclStmtNode::getSpan() const -> input::Span {
  return input::Span::combine(m_kw.getSpan(), m_fields.back().getIdentifier().getSpan());
}

auto StructDeclStmtNode::getId() const -> const lex::Token& { return m_id; }

auto StructDeclStmtNode::getFields() const -> const std::vector<FieldSpec>& { return m_fields; }

auto StructDeclStmtNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto StructDeclStmtNode::print(std::ostream& out) const -> std::ostream& {
  out << "struct-" << getIdOrErr(m_id) << '=';
  for (auto i = 0U; i < m_fields.size(); ++i) {
    if (i != 0) {
      out << ",";
    }
    out << getIdOrErr(m_fields[i].getType()) << '-' << getIdOrErr(m_fields[i].getIdentifier());
  }
  return out;
}

// Factories.
auto structDeclStmtNode(
    lex::Token kw,
    lex::Token id,
    lex::Token eq,
    std::vector<StructDeclStmtNode::FieldSpec> fields,
    std::vector<lex::Token> commas) -> NodePtr {
  if (fields.empty()) {
    throw std::invalid_argument{"Struct needs atleast one field"};
  }
  if (commas.size() != fields.size() - 1) {
    throw std::invalid_argument{"Incorrect number of commas"};
  }
  return std::unique_ptr<StructDeclStmtNode>{new StructDeclStmtNode{
      std::move(kw), std::move(id), std::move(eq), std::move(fields), std::move(commas)}};
}

} // namespace parse
