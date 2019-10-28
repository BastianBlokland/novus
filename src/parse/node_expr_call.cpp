#include "parse/node_expr_call.hpp"
#include "utilities.hpp"

namespace parse {

CallExprNode::CallExprNode(
    lex::Token func,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) :
    Node(NodeKind::ExprCall),
    m_func{std::move(func)},
    m_open{std::move(open)},
    m_args{std::move(args)},
    m_commas{std::move(commas)},
    m_close{std::move(close)} {}

auto CallExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const CallExprNode*>(&rhs);
  return r != nullptr && m_func == r->m_func && nodesEqual(m_args, r->m_args);
}

auto CallExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !CallExprNode::operator==(rhs);
}

auto CallExprNode::operator[](int i) const -> const Node& {
  if (i < 0 || static_cast<unsigned>(i) >= m_args.size()) {
    throw std::out_of_range("No child at given index");
  }
  return *m_args[i];
}

auto CallExprNode::getChildCount() const -> unsigned int { return m_args.size(); }

auto CallExprNode::getSpan() const -> lex::SourceSpan {
  return lex::SourceSpan::combine(m_func.getSpan(), m_close.getSpan());
}

auto CallExprNode::getFunc() const -> const lex::Token& { return m_func; }

auto CallExprNode::print(std::ostream& out) const -> std::ostream& {
  return out << "call-" << ::parse::getId(m_func).value_or("error");
}

// Factories.
auto callExprNode(
    lex::Token func,
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
  return std::unique_ptr<CallExprNode>{new CallExprNode{
      std::move(func), std::move(open), std::move(args), std::move(commas), std::move(close)}};
}

} // namespace parse
