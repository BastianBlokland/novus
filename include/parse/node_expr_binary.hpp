#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/node_type.hpp"
#include <memory>
#include <utility>

namespace parse {

class BinaryExprNode final : public Node {
public:
  BinaryExprNode() = delete;

  BinaryExprNode(NodePtr lhs, lex::Token op, NodePtr rhs) :
      Node(NodeType::ExprBinaryOp),
      m_lhs{std::move(lhs)},
      m_op{std::move(op)},
      m_rhs{std::move(rhs)} {

    if (m_lhs == nullptr) {
      throw std::invalid_argument("Lhs cannot be null");
    }
    if (m_rhs == nullptr) {
      throw std::invalid_argument("Rhs cannot be null");
    }
  }

  auto operator==(const Node* rhs) const noexcept -> bool override {
    const auto r = dynamic_cast<const BinaryExprNode*>(rhs);
    return r != nullptr && *m_lhs == r->m_lhs.get() && m_op == r->m_op && *m_rhs == r->m_rhs.get();
  }

  auto operator!=(const Node* rhs) const noexcept -> bool override {
    return !BinaryExprNode::operator==(rhs);
  }

  [[nodiscard]] auto operator[](int i) const -> Node& override {
    switch (i) {
    case 0:
      return *m_lhs.get();
    case 1:
      return *m_rhs.get();
    default:
      throw std::out_of_range("No child at given index");
    }
  }

  [[nodiscard]] auto getChildCount() const -> int override { return 2; }

  [[nodiscard]] auto clone() const -> NodePtr override {
    return std::make_unique<BinaryExprNode>(m_lhs->clone(), m_op, m_rhs->clone());
  }

private:
  const NodePtr m_lhs;
  const lex::Token m_op;
  const NodePtr m_rhs;

  auto print(std::ostream& out) const -> std::ostream& override { return out << m_op; }
};

// Factory.
inline auto binaryExprNode(NodePtr lhs, lex::Token op, NodePtr rhs) -> NodePtr {
  return std::make_unique<BinaryExprNode>(std::move(lhs), std::move(op), std::move(rhs));
}

} // namespace parse
