#pragma once
#include "prog/expr/node.hpp"
#include "prog/program.hpp"

namespace prog::expr {

// Fail the current executor. If the current executor is the main executor then the program will
// exit with exit-code 1. If the current executor is not the main then any executor that waits on
// the future of this executor will also fail.
class FailNode final : public Node {
  friend auto failNode(sym::TypeId type) -> NodePtr;

public:
  FailNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] constexpr static auto getKind() { return NodeKind::Fail; }

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;
  [[nodiscard]] auto toString() const -> std::string override;

  [[nodiscard]] auto clone(Rewriter* rewriter) const -> std::unique_ptr<Node> override;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  sym::TypeId m_type;

  explicit FailNode(sym::TypeId type);
};

// Factories.
auto failNode(sym::TypeId type) -> NodePtr;

} // namespace prog::expr
