#pragma once
#include "prog/expr/node.hpp"
#include "prog/program.hpp"

namespace prog::expr {

class LitLongNode final : public Node {
  friend auto litLongNode(const Program& program, int64_t val) -> NodePtr;

public:
  LitLongNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] constexpr static auto getKind() { return NodeKind::LitLong; }

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;
  [[nodiscard]] auto toString() const -> std::string override;

  [[nodiscard]] auto clone(Rewriter* rewriter) const -> std::unique_ptr<Node> override;

  [[nodiscard]] auto getVal() const noexcept -> int64_t;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  sym::TypeId m_type;
  int64_t m_val;

  LitLongNode(sym::TypeId type, int64_t val);
};

// Factories.
auto litLongNode(const Program& program, int64_t val) -> NodePtr;

} // namespace prog::expr
