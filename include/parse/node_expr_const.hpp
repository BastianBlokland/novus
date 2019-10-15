#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/node_type.hpp"
#include <memory>
#include <utility>

namespace parse {

class ConstExprNode final : public Node {
public:
  ConstExprNode() = delete;

  explicit ConstExprNode(lex::Token id) : Node(NodeType::ExprConst), m_id{std::move(id)} {}

  auto operator==(const Node& rhs) const noexcept -> bool override {
    const auto r = dynamic_cast<const ConstExprNode*>(&rhs);
    return r != nullptr && m_id == r->m_id;
  }

  auto operator!=(const Node& rhs) const noexcept -> bool override {
    return !ConstExprNode::operator==(rhs);
  }

  [[nodiscard]] auto operator[](int /*unused*/) const -> const Node& override {
    throw std::out_of_range("No child at given index");
  }

  [[nodiscard]] auto getChildCount() const -> int override { return 0; }

  [[nodiscard]] auto clone() const -> NodePtr override {
    return std::make_unique<ConstExprNode>(m_id);
  }

private:
  const lex::Token m_id;

  auto print(std::ostream& out) const -> std::ostream& override { return out << m_id; }
};

// Factories.
inline auto constExprNode(lex::Token id) -> NodePtr {
  return std::make_unique<ConstExprNode>(std::move(id));
}

} // namespace parse
