#include "parse/node_stmt_exec.hpp"
#include "utilities.hpp"

namespace parse {

ExecStmtNode::ExecStmtNode(
    lex::Token action,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) :
    Node(NodeKind::StmtExec),
    m_action{std::move(action)},
    m_open{std::move(open)},
    m_args{std::move(args)},
    m_commas{std::move(commas)},
    m_close{std::move(close)} {}

auto ExecStmtNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const ExecStmtNode*>(&rhs);
  return r != nullptr && m_action == r->m_action && nodesEqual(m_args, r->m_args);
}

auto ExecStmtNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !ExecStmtNode::operator==(rhs);
}

auto ExecStmtNode::operator[](int i) const -> const Node& {
  if (i < 0 || static_cast<unsigned>(i) >= m_args.size()) {
    throw std::out_of_range("No child at given index");
  }
  return *m_args[i];
}

auto ExecStmtNode::getChildCount() const -> unsigned int { return m_args.size(); }

auto ExecStmtNode::getSpan() const -> input::Span {
  return input::Span::combine(m_action.getSpan(), m_close.getSpan());
}

auto ExecStmtNode::getAction() const -> const lex::Token& { return m_action; }

auto ExecStmtNode::print(std::ostream& out) const -> std::ostream& {
  return out << "exec-" << ::parse::getId(m_action).value_or("error");
}

// Factories.
auto execStmtNode(
    lex::Token action,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) -> NodePtr {
  if (anyNodeNull(args)) {
    throw std::invalid_argument("args cannot contain a nullptr");
  }
  if (args.empty() ? !commas.empty() : commas.size() != args.size() - 1) {
    throw std::invalid_argument("Incorrect number of commas");
  }
  return std::unique_ptr<ExecStmtNode>{new ExecStmtNode{
      std::move(action), std::move(open), std::move(args), std::move(commas), std::move(close)}};
}

} // namespace parse
