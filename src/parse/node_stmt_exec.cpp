#include "parse/node_stmt_exec.hpp"
#include "utilities.hpp"

namespace parse {

ExecStmtNode::ExecStmtNode(
    lex::Token target,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) :
    m_target{std::move(target)},
    m_open{std::move(open)},
    m_args{std::move(args)},
    m_commas{std::move(commas)},
    m_close{std::move(close)} {}

auto ExecStmtNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const ExecStmtNode*>(&rhs);
  return r != nullptr && m_target == r->m_target && nodesEqual(m_args, r->m_args);
}

auto ExecStmtNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !ExecStmtNode::operator==(rhs);
}

auto ExecStmtNode::operator[](unsigned int i) const -> const Node& {
  if (i >= m_args.size()) {
    throw std::out_of_range("No child at given index");
  }
  return *m_args[i];
}

auto ExecStmtNode::getChildCount() const -> unsigned int { return m_args.size(); }

auto ExecStmtNode::getSpan() const -> input::Span {
  return input::Span::combine(m_target.getSpan(), m_close.getSpan());
}

auto ExecStmtNode::getTarget() const -> const lex::Token& { return m_target; }

auto ExecStmtNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto ExecStmtNode::print(std::ostream& out) const -> std::ostream& {
  return out << "exec-" << ::parse::getId(m_target).value_or("error");
}

// Factories.
auto execStmtNode(
    lex::Token target,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) -> NodePtr {
  if (anyNodeNull(args)) {
    throw std::invalid_argument{"args cannot contain a nullptr"};
  }
  if (args.empty() ? !commas.empty() : commas.size() != args.size() - 1) {
    throw std::invalid_argument{"Incorrect number of commas"};
  }
  return std::unique_ptr<ExecStmtNode>{new ExecStmtNode{
      std::move(target), std::move(open), std::move(args), std::move(commas), std::move(close)}};
}

} // namespace parse
