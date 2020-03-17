#pragma once
#include "prog/expr/node.hpp"
#include "prog/program.hpp"

namespace prog::expr {

class ClosureNode final : public Node {
  friend auto closureNode(
      const Program& program, sym::TypeId type, sym::FuncId func, std::vector<NodePtr> boundArgs)
      -> NodePtr;

public:
  ClosureNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] constexpr static auto getKind() { return NodeKind::Closure; }

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;
  [[nodiscard]] auto toString() const -> std::string override;

  [[nodiscard]] auto clone(Rewriter* rewriter) const -> std::unique_ptr<Node> override;

  [[nodiscard]] auto getFunc() const noexcept -> sym::FuncId;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  sym::TypeId m_type;
  sym::FuncId m_func;
  std::vector<NodePtr> m_boundArgs;

  ClosureNode(sym::TypeId type, sym::FuncId func, std::vector<NodePtr> boundArgs);
};

// Factories.
auto closureNode(
    const Program& program, sym::TypeId type, sym::FuncId func, std::vector<NodePtr> boundArgs)
    -> NodePtr;

} // namespace prog::expr
