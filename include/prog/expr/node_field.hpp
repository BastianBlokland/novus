#pragma once
#include "prog/expr/node.hpp"
#include "prog/program.hpp"
#include "prog/sym/field_decl_table.hpp"

namespace prog::expr {

class FieldExprNode final : public Node {
  friend auto fieldExprNode(const Program& prog, NodePtr lhs, sym::FieldId id) -> NodePtr;

public:
  FieldExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] constexpr static auto getKind() { return NodeKind::Field; }

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;
  [[nodiscard]] auto toString() const -> std::string override;

  [[nodiscard]] auto clone(Rewriter* rewriter) const -> std::unique_ptr<Node> override;

  [[nodiscard]] auto getId() const noexcept -> sym::FieldId;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  NodePtr m_lhs;
  sym::FieldId m_id;
  sym::TypeId m_type;

  FieldExprNode(NodePtr lhs, sym::FieldId id, sym::TypeId type);
};

using FieldNodePtr = std::unique_ptr<FieldExprNode>;

// Factories.
auto fieldExprNode(const Program& prog, NodePtr lhs, sym::FieldId id) -> NodePtr;

} // namespace prog::expr
