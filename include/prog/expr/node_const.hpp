#pragma once
#include "prog/expr/node.hpp"
#include "prog/sym/const_decl_table.hpp"

namespace prog::expr {

// Load the value of a constant.
class ConstExprNode final : public Node {
  friend auto constExprNode(const sym::ConstDeclTable& constTable, sym::ConstId id) -> NodePtr;

public:
  ConstExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] constexpr static auto getKind() { return NodeKind::Const; }

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;
  [[nodiscard]] auto toString() const -> std::string override;

  [[nodiscard]] auto clone(Rewriter* rewriter) const -> std::unique_ptr<Node> override;

  [[nodiscard]] auto getId() const noexcept -> sym::ConstId;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  sym::ConstId m_id;
  sym::TypeId m_type;

  ConstExprNode(sym::ConstId id, sym::TypeId type);
};

using ConstNodePtr = std::unique_ptr<ConstExprNode>;

// Factories.
auto constExprNode(const sym::ConstDeclTable& constTable, sym::ConstId id) -> NodePtr;

} // namespace prog::expr
