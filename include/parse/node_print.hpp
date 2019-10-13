#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/node_type.hpp"
#include <memory>
#include <utility>

namespace parse {

class PrintNode final : public Node {
public:
  PrintNode() = delete;

  PrintNode(lex::Token kw, std::unique_ptr<Node> expr) :
      Node(NodeType::StmtPrint), m_kw{std::move(kw)}, m_expr{std::move(expr)} {}

  auto operator==(const Node* rhs) const noexcept -> bool override {
    const auto castedRhs = dynamic_cast<const PrintNode*>(rhs);
    return castedRhs != nullptr && m_expr == castedRhs->m_expr;
  }

  auto operator!=(const Node* rhs) const noexcept -> bool override {
    return !PrintNode::operator==(rhs);
  }

  [[nodiscard]] auto clone() const -> std::unique_ptr<Node> override {
    return std::make_unique<PrintNode>(m_kw, m_expr->clone());
  }

private:
  const lex::Token m_kw;
  const std::unique_ptr<Node> m_expr;

  auto print(std::ostream& out) const -> std::ostream& override { return out << "print"; }
};

// Factory.
inline auto printNode(lex::Token kw, std::unique_ptr<Node> expr) -> std::unique_ptr<Node> {
  return std::make_unique<PrintNode>(std::move(kw), std::move(expr));
}

} // namespace parse
