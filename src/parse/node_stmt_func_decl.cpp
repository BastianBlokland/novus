#include "parse/node_stmt_func_decl.hpp"
#include "utilities.hpp"

namespace parse {

static auto getIdOrErr(const lex::Token& token) {
  return getId(token).value_or(std::string("err"));
}

FuncDeclStmtNode::FuncDeclStmtNode(
    lex::Token kw,
    lex::Token id,
    std::optional<TypeSubstitutionList> typeSubs,
    ArgumentListDecl argList,
    std::optional<RetTypeSpec> retType,
    NodePtr body) :
    m_kw{std::move(kw)},
    m_id{std::move(id)},
    m_typeSubs{std::move(typeSubs)},
    m_argList{std::move(argList)},
    m_retType{std::move(retType)},
    m_body{std::move(body)} {}

auto FuncDeclStmtNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const FuncDeclStmtNode*>(&rhs);
  return r != nullptr && m_id == r->m_id && m_argList == r->m_argList &&
      m_retType == r->m_retType && *m_body == *r->m_body;
}

auto FuncDeclStmtNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !FuncDeclStmtNode::operator==(rhs);
}

auto FuncDeclStmtNode::operator[](unsigned int i) const -> const Node& {
  const auto initializerCount = m_argList.getInitializerCount();
  if (i < initializerCount) {
    return m_argList.getInitializer(i);
  }
  if (i == initializerCount) {
    return *m_body;
  }
  throw std::out_of_range{"No child at given index"};
}

auto FuncDeclStmtNode::getChildCount() const -> unsigned int {
  return m_argList.getInitializerCount() + 1;
}

auto FuncDeclStmtNode::getSpan() const -> input::Span {
  return input::Span::combine(m_kw.getSpan(), m_body->getSpan());
}

auto FuncDeclStmtNode::isAction() const -> bool { return getKw(m_kw) == lex::Keyword::Act; }

auto FuncDeclStmtNode::getId() const -> const lex::Token& { return m_id; }

auto FuncDeclStmtNode::getTypeSubs() const -> const std::optional<TypeSubstitutionList>& {
  return m_typeSubs;
}

auto FuncDeclStmtNode::getArgList() const -> const ArgumentListDecl& { return m_argList; }

auto FuncDeclStmtNode::getRetType() const -> const std::optional<RetTypeSpec>& { return m_retType; }

auto FuncDeclStmtNode::getBody() const -> const Node& { return *m_body; }

auto FuncDeclStmtNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto FuncDeclStmtNode::print(std::ostream& out) const -> std::ostream& {
  out << (isAction() ? "act-" : "fun-");
  if (m_id.getKind() == lex::TokenKind::Identifier) {
    out << getIdOrErr(m_id);
  } else {
    out << "op-" << m_id;
  }
  if (m_typeSubs) {
    out << *m_typeSubs;
  }
  out << m_argList;
  if (m_retType) {
    out << *m_retType;
  }
  return out;
}

// Factories.
auto funcDeclStmtNode(
    lex::Token kw,
    lex::Token id,
    std::optional<TypeSubstitutionList> typeSubs,
    ArgumentListDecl argList,
    std::optional<RetTypeSpec> retType,
    NodePtr body) -> NodePtr {

  if (body == nullptr) {
    throw std::invalid_argument{"Body cannot be null"};
  }
  return std::unique_ptr<FuncDeclStmtNode>{new FuncDeclStmtNode{
      std::move(kw),
      std::move(id),
      std::move(typeSubs),
      std::move(argList),
      std::move(retType),
      std::move(body)}};
}

} // namespace parse
