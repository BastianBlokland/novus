#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/node_type.hpp"
#include <memory>
#include <utility>

namespace parse {

class LitExprNode final : public Node {
public:
  LitExprNode() = delete;

  explicit LitExprNode(lex::Token val) : Node(NodeType::ExprLit), m_val{std::move(val)} {}

  auto operator==(const Node* rhs) const noexcept -> bool override {
    const auto r = dynamic_cast<const LitExprNode*>(rhs);
    return r != nullptr && m_val == r->m_val;
  }

  auto operator!=(const Node* rhs) const noexcept -> bool override {
    return !LitExprNode::operator==(rhs);
  }

  [[nodiscard]] auto clone() const -> NodePtr override {
    return std::make_unique<LitExprNode>(m_val);
  }

private:
  const lex::Token m_val;

  auto print(std::ostream& out) const -> std::ostream& override { return out << "expr-literal"; }
};

// Factory.
inline auto litExprNode(lex::Token val) -> NodePtr {
  return std::make_unique<LitExprNode>(std::move(val));
}

} // namespace parse
