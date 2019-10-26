#pragma once
#include "prog/expr/node.hpp"
#include "prog/sym/const_decl_table.hpp"

namespace prog::expr {

class ConstExprNode final : public Node {
  friend auto constExprNode(const sym::ConstDeclTable& constTable, sym::ConstId id) -> NodePtr;

public:
  ConstExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;

private:
  sym::ConstId m_id;
  sym::TypeId m_type;

  ConstExprNode(sym::ConstId id, sym::TypeId type);
};

using ConstNodePtr = std::unique_ptr<ConstExprNode>;

// Factories.
auto constExprNode(const sym::ConstDeclTable& constTable, sym::ConstId id) -> NodePtr;

} // namespace prog::expr
