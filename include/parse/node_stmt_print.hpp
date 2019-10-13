#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/node_type.hpp"
#include <memory>
#include <utility>

namespace parse {

class PrintStmtNode final : public Node {
public:
  PrintStmtNode() = delete;

  PrintStmtNode(lex::Token kw, NodePtr expr) :
      Node(NodeType::StmtPrint), m_kw{std::move(kw)}, m_expr{std::move(expr)} {}

  auto operator==(const Node* rhs) const noexcept -> bool override {
    const auto castedRhs = dynamic_cast<const PrintStmtNode*>(rhs);
    return castedRhs != nullptr && m_expr == castedRhs->m_expr;
  }

  auto operator!=(const Node* rhs) const noexcept -> bool override {
    return !PrintStmtNode::operator==(rhs);
  }

  [[nodiscard]] auto clone() const -> NodePtr override {
    return std::make_unique<PrintStmtNode>(m_kw, m_expr->clone());
  }

private:
  const lex::Token m_kw;
  const NodePtr m_expr;

  auto print(std::ostream& out) const -> std::ostream& override { return out << "print"; }
};

// Factory.
inline auto printStmtNode(lex::Token kw, NodePtr expr) -> NodePtr {
  return std::make_unique<PrintStmtNode>(std::move(kw), std::move(expr));
}

} // namespace parse
