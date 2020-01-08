#pragma once
#include "prog/expr/node.hpp"
#include "prog/program.hpp"

namespace prog::expr {

class LitCharNode final : public Node {
  friend auto litCharNode(const Program& program, char val) -> NodePtr;

public:
  LitCharNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;
  [[nodiscard]] auto toString() const -> std::string override;

  [[nodiscard]] auto getVal() const noexcept -> char;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  sym::TypeId m_type;
  char m_val;

  LitCharNode(sym::TypeId type, char val);
};

// Factories.
auto litCharNode(const Program& program, char val) -> NodePtr;

} // namespace prog::expr
