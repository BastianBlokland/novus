#pragma once
#include "lex/token.hpp"
#include "lex/token_type.hpp"
#include "parse/node.hpp"
#include "parse/node_type.hpp"
#include <memory>
#include <utility>

namespace parse {

class ConstDeclExprNode final : public Node {
public:
  ConstDeclExprNode() = delete;

  explicit ConstDeclExprNode(lex::Token id, lex::Token eq, NodePtr rhs) :
      Node(NodeType::ExprConstDecl),
      m_id{std::move(id)},
      m_eq{std::move(eq)},
      m_rhs{std::move(rhs)} {

    if (m_rhs == nullptr) {
      throw std::invalid_argument("Rhs cannot be null");
    }
  }

  auto operator==(const Node& rhs) const noexcept -> bool override {
    const auto r = dynamic_cast<const ConstDeclExprNode*>(&rhs);
    return r != nullptr && m_id == r->m_id && *m_rhs == *r->m_rhs;
  }

  auto operator!=(const Node& rhs) const noexcept -> bool override {
    return !ConstDeclExprNode::operator==(rhs);
  }

  [[nodiscard]] auto operator[](int i) const -> const Node& override {
    switch (i) {
    case 0:
      return *m_rhs;
    default:
      throw std::out_of_range("No child at given index");
    }
  }

  [[nodiscard]] auto getChildCount() const -> int override { return 1; }

  [[nodiscard]] auto clone() const -> NodePtr override {
    return std::make_unique<ConstDeclExprNode>(m_id, m_eq, m_rhs->clone());
  }

private:
  const lex::Token m_id;
  const lex::Token m_eq;
  const NodePtr m_rhs;

  auto print(std::ostream& out) const -> std::ostream& override { return out << "decl-" << m_id; }
};

// Factories.
inline auto constDeclExprNode(lex::Token id, lex::Token eq, NodePtr rhs) -> NodePtr {
  return std::make_unique<ConstDeclExprNode>(std::move(id), std::move(eq), std::move(rhs));
}

} // namespace parse
