#pragma once
#include "prog/expr/node.hpp"
#include "prog/program.hpp"

namespace prog::expr {

// Load a float literal.
class LitFloatNode final : public Node {
  friend auto litFloatNode(const Program& program, float val) -> NodePtr;

public:
  LitFloatNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] constexpr static auto getKind() { return NodeKind::LitFloat; }

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;
  [[nodiscard]] auto toString() const -> std::string override;

  [[nodiscard]] auto clone(Rewriter* rewriter) const -> std::unique_ptr<Node> override;

  [[nodiscard]] auto getVal() const noexcept -> float;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  sym::TypeId m_type;
  float m_val;

  LitFloatNode(sym::TypeId type, float val);
};

// Factories.
auto litFloatNode(const Program& program, float val) -> NodePtr;

} // namespace prog::expr
