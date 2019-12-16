#include "parse/node_stmt_union_decl.hpp"
#include "utilities.hpp"

namespace parse {

static auto getIdOrErr(const lex::Token& token) {
  return getId(token).value_or(std::string("err"));
}
UnionDeclStmtNode::UnionDeclStmtNode(
    lex::Token kw,
    lex::Token id,
    std::optional<TypeSubstitutionList> typeSubs,
    lex::Token eq,
    std::vector<Type> types,
    std::vector<lex::Token> commas) :
    m_kw{std::move(kw)},
    m_id{std::move(id)},
    m_typeSubs{std::move(typeSubs)},
    m_eq{std::move(eq)},
    m_types{std::move(types)},
    m_commas{std::move(commas)} {}

auto UnionDeclStmtNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const UnionDeclStmtNode*>(&rhs);
  return r != nullptr && m_id == r->m_id && m_types == r->m_types;
}

auto UnionDeclStmtNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !UnionDeclStmtNode::operator==(rhs);
}

auto UnionDeclStmtNode::operator[](unsigned int /*unused*/) const -> const Node& {
  throw std::out_of_range{"No child at given index"};
}

auto UnionDeclStmtNode::getChildCount() const -> unsigned int { return 0; }

auto UnionDeclStmtNode::getSpan() const -> input::Span {
  return input::Span::combine(m_kw.getSpan(), m_types.back().getSpan());
}

auto UnionDeclStmtNode::getId() const -> const lex::Token& { return m_id; }

auto UnionDeclStmtNode::getTypeSubs() const -> const std::optional<TypeSubstitutionList>& {
  return m_typeSubs;
}

auto UnionDeclStmtNode::getTypes() const -> const std::vector<Type>& { return m_types; }

auto UnionDeclStmtNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto UnionDeclStmtNode::print(std::ostream& out) const -> std::ostream& {
  out << "union-" << getIdOrErr(m_id);
  if (m_typeSubs) {
    out << *m_typeSubs;
  }
  out << '=';
  for (auto i = 0U; i < m_types.size(); ++i) {
    if (i != 0) {
      out << ",";
    }
    out << m_types[i];
  }
  return out;
}

// Factories.
auto unionDeclStmtNode(
    lex::Token kw,
    lex::Token id,
    std::optional<TypeSubstitutionList> typeSubs,
    lex::Token eq,
    std::vector<Type> types,
    std::vector<lex::Token> commas) -> NodePtr {
  if (types.size() < 2) {
    throw std::invalid_argument{"Union needs at least two types"};
  }
  if (commas.size() != types.size() - 1) {
    throw std::invalid_argument{"Incorrect number of commas"};
  }
  return std::unique_ptr<UnionDeclStmtNode>{new UnionDeclStmtNode{std::move(kw),
                                                                  std::move(id),
                                                                  std::move(typeSubs),
                                                                  std::move(eq),
                                                                  std::move(types),
                                                                  std::move(commas)}};
}

} // namespace parse
