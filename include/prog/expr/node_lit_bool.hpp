#pragma once
#include "prog/expr/node.hpp"
#include "prog/program.hpp"

namespace prog::expr {

class LitBoolNode final : public Node {
  friend auto litBoolNode(const Program& program, bool val) -> NodePtr;

public:
  LitBoolNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;
  [[nodiscard]] auto toString() const -> std::string override;

  [[nodiscard]] auto getVal() const noexcept -> bool;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  sym::TypeId m_type;
  bool m_val;

  LitBoolNode(sym::TypeId type, bool val);
};

// Factories.
auto litBoolNode(const Program& program, bool val) -> NodePtr;

} // namespace prog::expr
