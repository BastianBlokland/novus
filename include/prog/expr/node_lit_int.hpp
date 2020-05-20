#pragma once
#include "prog/expr/node.hpp"
#include "prog/program.hpp"

namespace prog::expr {

// Load an integer literal.
class LitIntNode final : public Node {
  friend auto litIntNode(const Program& program, int32_t val) -> NodePtr;

public:
  LitIntNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] constexpr static auto getKind() { return NodeKind::LitInt; }

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;
  [[nodiscard]] auto toString() const -> std::string override;

  [[nodiscard]] auto clone(Rewriter* rewriter) const -> std::unique_ptr<Node> override;

  [[nodiscard]] auto getVal() const noexcept -> int32_t;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  sym::TypeId m_type;
  int32_t m_val;

  LitIntNode(sym::TypeId type, int32_t val);
};

// Factories.
auto litIntNode(const Program& program, int32_t val) -> NodePtr;

} // namespace prog::expr
