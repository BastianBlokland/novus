#pragma once
#include "prog/expr/node.hpp"
#include "prog/program.hpp"

namespace prog::expr {

class LitCharNode final : public Node {
  friend auto litCharNode(const Program& program, uint8_t val) -> NodePtr;

public:
  LitCharNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] constexpr static auto getKind() { return NodeKind::LitChar; }

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;
  [[nodiscard]] auto toString() const -> std::string override;

  [[nodiscard]] auto clone(Rewriter* rewriter) const -> std::unique_ptr<Node> override;

  [[nodiscard]] auto getVal() const noexcept -> uint8_t;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  sym::TypeId m_type;
  uint8_t m_val;

  LitCharNode(sym::TypeId type, uint8_t val);
};

// Factories.
auto litCharNode(const Program& program, uint8_t val) -> NodePtr;

} // namespace prog::expr
