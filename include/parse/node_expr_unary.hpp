#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/node_type.hpp"
#include <memory>
#include <utility>

namespace parse {

class UnaryExprNode final : public Node {
public:
  UnaryExprNode() = delete;

  UnaryExprNode(lex::Token op, NodePtr rhs) :
      Node(NodeType::ExprUnaryOp), m_op{std::move(op)}, m_rhs{std::move(rhs)} {

    if (rhs == nullptr) {
      throw std::invalid_argument("Rhs cannot be null");
    }
  }

  auto operator==(const Node* rhs) const noexcept -> bool override {
    const auto r = dynamic_cast<const UnaryExprNode*>(rhs);
    return r != nullptr && m_op == r->m_op && *m_rhs == r->m_rhs.get();
  }

  auto operator!=(const Node* rhs) const noexcept -> bool override {
    return !UnaryExprNode::operator==(rhs);
  }

  [[nodiscard]] auto clone() const -> NodePtr override {
    return std::make_unique<UnaryExprNode>(m_op, m_rhs->clone());
  }

private:
  const lex::Token m_op;
  const NodePtr m_rhs;

  auto print(std::ostream& out) const -> std::ostream& override { return out << "expr-unary"; }
};

// Factory.
inline auto unaryExprNode(lex::Token op, NodePtr rhs) -> NodePtr {
  return std::make_unique<UnaryExprNode>(std::move(op), std::move(rhs));
}

} // namespace parse
