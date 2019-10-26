#pragma once
#include "prog/expr/node.hpp"
#include "prog/sym/type_decl_table.hpp"

namespace prog::expr {

class LitIntNode final : public Node {
  friend auto litIntNode(const sym::TypeDeclTable& typeTable, int32_t val) -> NodePtr;

public:
  LitIntNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;

private:
  sym::TypeId m_type;
  int32_t m_val;

  LitIntNode(sym::TypeId type, int32_t val);
};

// Factories.
auto litIntNode(const sym::TypeDeclTable& typeTable, int32_t val) -> NodePtr;

} // namespace prog::expr
