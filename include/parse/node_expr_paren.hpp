#pragma once
#include "lex/token.hpp"
#include "lex/token_type.hpp"
#include "parse/node.hpp"
#include "parse/node_type.hpp"
#include <memory>
#include <utility>

namespace parse {

class ParenExprNode final : public Node {
public:
  ParenExprNode() = delete;

  ParenExprNode(lex::Token open, NodePtr expr, lex::Token close) :
      Node(NodeType::ExprParen),
      m_open{std::move(open)},
      m_expr{std::move(expr)},
      m_close{std::move(close)} {

    if (m_open.getType() != lex::TokenType::SepOpenParen) {
      throw std::invalid_argument("Open token is not a opening parentheses");
    }
    if (m_expr == nullptr) {
      throw std::invalid_argument("Expr cannot be null");
    }
    if (m_close.getType() != lex::TokenType::SepCloseParen) {
      throw std::invalid_argument("Close token is not a closing parentheses");
    }
  }

  auto operator==(const Node& rhs) const noexcept -> bool override {
    const auto r = dynamic_cast<const ParenExprNode*>(&rhs);
    return r != nullptr && m_open == r->m_open && *m_expr == *r->m_expr && m_close == r->m_close;
  }

  auto operator!=(const Node& rhs) const noexcept -> bool override {
    return !ParenExprNode::operator==(rhs);
  }

  [[nodiscard]] auto operator[](int i) const -> const Node& override {
    switch (i) {
    case 0:
      return *m_expr;
    default:
      throw std::out_of_range("No child at given index");
    }
  }

  [[nodiscard]] auto getChildCount() const -> int override { return 1; }

  [[nodiscard]] auto clone() const -> NodePtr override {
    return std::make_unique<ParenExprNode>(m_open, m_expr->clone(), m_close);
  }

private:
  const lex::Token m_open;
  const NodePtr m_expr;
  const lex::Token m_close;

  auto print(std::ostream& out) const -> std::ostream& override { return out << "parenthesized"; }
};

// Factories.
inline auto parenExprNode(lex::Token open, NodePtr expr, lex::Token close) -> NodePtr {
  return std::make_unique<ParenExprNode>(std::move(open), std::move(expr), std::move(close));
}

} // namespace parse
