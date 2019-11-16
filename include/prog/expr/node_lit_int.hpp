#pragma once
#include "prog/expr/node.hpp"
#include "prog/program.hpp"

namespace prog::expr {

class LitIntNode final : public Node {
  friend auto litIntNode(const Program& program, int32_t val) -> NodePtr;

public:
  LitIntNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;

  [[nodiscard]] auto getVal() const noexcept -> int32_t;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  sym::TypeId m_type;
  int32_t m_val;

  LitIntNode(sym::TypeId type, int32_t val);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto litIntNode(const Program& program, int32_t val) -> NodePtr;

} // namespace prog::expr
