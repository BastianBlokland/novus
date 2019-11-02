#include "parse/node_stmt_func_decl.hpp"
#include "utilities.hpp"

namespace parse {

static auto getIdOrErr(const lex::Token& token) {
  return getId(token).value_or(std::string("err"));
}

FuncDeclStmtNode::FuncDeclStmtNode(
    lex::Token kw,
    lex::Token id,
    lex::Token open,
    std::vector<arg> args,
    std::vector<lex::Token> commas,
    lex::Token close,
    lex::Token arrow,
    lex::Token retType,
    NodePtr body) :
    Node(NodeKind::StmtFuncDecl),
    m_kw{std::move(kw)},
    m_id{std::move(id)},
    m_open{std::move(open)},
    m_args{std::move(args)},
    m_commas{std::move(commas)},
    m_close{std::move(close)},
    m_arrow{std::move(arrow)},
    m_retType{std::move(retType)},
    m_body{std::move(body)} {}

auto FuncDeclStmtNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const FuncDeclStmtNode*>(&rhs);
  return r != nullptr && m_id == r->m_id && m_args == r->m_args && m_retType == r->m_retType &&
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

auto FuncDeclStmtNode::getChildCount() const -> unsigned int { return 1; }

auto FuncDeclStmtNode::getSpan() const -> input::Span {
  return input::Span::combine(m_kw.getSpan(), m_body->getSpan());
}

auto FuncDeclStmtNode::getId() const -> const lex::Token& { return m_id; }

auto FuncDeclStmtNode::getArgs() const -> const std::vector<arg>& { return m_args; }

auto FuncDeclStmtNode::getRetType() const -> const lex::Token& { return m_retType; }

auto FuncDeclStmtNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto FuncDeclStmtNode::print(std::ostream& out) const -> std::ostream& {
  out << "fun-" << getIdOrErr(m_id) << '(';
  for (auto i = 0U; i < m_args.size(); ++i) {
    if (i != 0) {
      out << ",";
    }
    out << getIdOrErr(m_args[i].first) << '-' << getIdOrErr(m_args[i].second);
  }
  return out << ")->" << getIdOrErr(m_retType);
}

// Factories.
auto funcDeclStmtNode(
    lex::Token kw,
    lex::Token id,
    lex::Token open,
    std::vector<FuncDeclStmtNode::arg> args,
    std::vector<lex::Token> commas,
    lex::Token close,
    lex::Token arrow,
    lex::Token retType,
    NodePtr body) -> NodePtr {
  if (args.empty() ? !commas.empty() : commas.size() != args.size() - 1) {
    throw std::invalid_argument("Incorrect number of commas");
  }
  if (body == nullptr) {
    throw std::invalid_argument("Body cannot be null");
  }
  return std::unique_ptr<FuncDeclStmtNode>{new FuncDeclStmtNode{std::move(kw),
                                                                std::move(id),
                                                                std::move(open),
                                                                std::move(args),
                                                                std::move(commas),
                                                                std::move(close),
                                                                std::move(arrow),
                                                                std::move(retType),
                                                                std::move(body)}};
}

} // namespace parse
