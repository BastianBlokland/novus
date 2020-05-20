#pragma once
#include "prog/expr/call_mode.hpp"
#include "prog/expr/node.hpp"
#include "prog/program.hpp"

namespace prog::expr {

// Execute a dynamic call to the result of an expression.
// Note: Expression result type has to be a function literal or a closure object.
// Note: For forked calls the return type has to be a future type.
// Note: For lazy calls the return type has to be a lazy type.
//
class CallDynExprNode final : public Node {
  friend auto callDynExprNode(const Program& prog, NodePtr lhs, std::vector<NodePtr> args)
      -> NodePtr;
  friend auto callDynExprNode(
      const Program& prog,
      NodePtr lhs,
      sym::TypeId resultType,
      std::vector<NodePtr> args,
      CallMode mode) -> NodePtr;

public:
  CallDynExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] constexpr static auto getKind() { return NodeKind::CallDyn; }

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;
  [[nodiscard]] auto toString() const -> std::string override;

  [[nodiscard]] auto clone(Rewriter* rewriter) const -> std::unique_ptr<Node> override;

  [[nodiscard]] auto getArgs() const noexcept -> const std::vector<NodePtr>&;
  [[nodiscard]] auto getMode() const noexcept -> CallMode;
  [[nodiscard]] auto isFork() const noexcept -> bool;
  [[nodiscard]] auto isLazy() const noexcept -> bool;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  NodePtr m_lhs;
  sym::TypeId m_resultType;
  std::vector<NodePtr> m_args;
  CallMode m_mode;

  CallDynExprNode(NodePtr lhs, sym::TypeId resultType, std::vector<NodePtr> args, CallMode mode);
};

// Factories.
auto callDynExprNode(const Program& prog, NodePtr lhs, std::vector<NodePtr> args) -> NodePtr;
auto callDynExprNode(
    const Program& prog,
    NodePtr lhs,
    sym::TypeId resultType,
    std::vector<NodePtr> args,
    CallMode mode) -> NodePtr;

} // namespace prog::expr
