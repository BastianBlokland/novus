#include "parse/node_stmt_func_decl.hpp"
#include "utilities.hpp"

namespace parse {

static auto getIdOrErr(const lex::Token& token) {
  return getId(token).value_or(std::string("err"));
}

FuncDeclStmtNode::FuncDeclStmtNode(
    lex::Token retType,
    lex::Token id,
    lex::Token open,
    std::vector<std::pair<lex::Token, lex::Token>> args,
    std::vector<lex::Token> commas,
    lex::Token close,
    NodePtr body) :

    Node(NodeType::StmtFuncDecl),
    m_retType{std::move(retType)},
    m_id{std::move(id)},
    m_open{std::move(open)},
    m_args{std::move(args)},
    m_commas{std::move(commas)},
    m_close{std::move(close)},
    m_body{std::move(body)} {

  if (m_args.empty() ? !m_commas.empty() : m_commas.size() != m_args.size() - 1) {
    throw std::invalid_argument("Incorrect number of commas");
  }
  if (m_body == nullptr) {
    throw std::invalid_argument("Body cannot be null");
  }
}

auto FuncDeclStmtNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const FuncDeclStmtNode*>(&rhs);
  return r != nullptr && m_retType == r->m_retType && m_id == r->m_id &&
      m_args.size() == r->m_args.size() &&
      std::equal(
             m_args.begin(),
             m_args.end(),
             r->m_args.begin(),
             [](const auto& l, const auto& r) {
               return l.first == r.first && l.second == r.second;
             }) &&
      *m_body == *r->m_body;
}

auto FuncDeclStmtNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !FuncDeclStmtNode::operator==(rhs);
}

auto FuncDeclStmtNode::operator[](int i) const -> const Node& {
  if (i == 0) {
    return *m_body;
  }
  throw std::out_of_range("No child at given index");
}

auto FuncDeclStmtNode::getChildCount() const -> int { return 1; }

auto FuncDeclStmtNode::print(std::ostream& out) const -> std::ostream& {
  out << getIdOrErr(m_retType) << '-' << getIdOrErr(m_id) << '(';
  for (auto i = 0U; i < m_args.size(); ++i) {
    if (i != 0) {
      out << ",";
    }
    out << getIdOrErr(m_args[i].first) << '-' << getIdOrErr(m_args[i].second);
  }
  return out << ')';
}

// Factories.
auto funcDeclStmtNode(
    lex::Token retType,
    lex::Token id,
    lex::Token open,
    std::vector<std::pair<lex::Token, lex::Token>> args,
    std::vector<lex::Token> commas,
    lex::Token close,
    NodePtr body) -> NodePtr {
  return std::make_unique<FuncDeclStmtNode>(
      std::move(retType),
      std::move(id),
      std::move(open),
      std::move(args),
      std::move(commas),
      std::move(close),
      std::move(body));
}

} // namespace parse
