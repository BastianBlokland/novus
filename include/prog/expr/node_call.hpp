#pragma once
#include "prog/expr/call_mode.hpp"
#include "prog/expr/node.hpp"
#include "prog/program.hpp"
#include <functional>

namespace prog::expr {

// Execute a static call to a function.
// Note: For forked calls the return type has to be a future type.
// Note: For lazy calls the return type has to be a lazy type.
//
class CallExprNode final : public Node {
  friend auto callExprNode(const Program& prog, sym::FuncId func, std::vector<NodePtr> args)
      -> NodePtr;
  friend auto callExprNode(
      const Program& prog,
      sym::FuncId func,
      sym::TypeId resultType,
      std::vector<NodePtr> args,
      CallMode mode) -> NodePtr;

public:
  using ArgPatcher = std::function<void(sym::FuncId func, std::vector<NodePtr>* args)>;

  CallExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] constexpr static auto getKind() { return NodeKind::Call; }

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;
  [[nodiscard]] auto toString() const -> std::string override;

  [[nodiscard]] auto clone(Rewriter* rewriter) const -> std::unique_ptr<Node> override;

  [[nodiscard]] auto getFunc() const noexcept -> sym::FuncId;
  [[nodiscard]] auto getArgs() const noexcept -> const std::vector<NodePtr>&;
  [[nodiscard]] auto getMode() const noexcept -> CallMode;
  [[nodiscard]] auto isFork() const noexcept -> bool;
  [[nodiscard]] auto isLazy() const noexcept -> bool;
  [[nodiscard]] auto isComplete(const Program& prog) const noexcept -> bool;

  /* Patching arguments actually mutates the arguments. This only ever valid while still
   * constructing a program. After sharing your program this should never be called.
   *
   * In general using the rewriting api is a better way to modify programs after creation.
   */
  auto patchArgs(ArgPatcher patcher) const -> void;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  sym::FuncId m_func;
  sym::TypeId m_resultType;
  CallMode m_mode;

  // NOTE: The arguments are mutable because optional arguments are applied in a separate phase
  // after all functions have been defined.
  mutable std::vector<NodePtr> m_args;

  CallExprNode(sym::FuncId func, sym::TypeId resultType, std::vector<NodePtr> args, CallMode mode);
};

// Factories.
auto callExprNode(const Program& prog, sym::FuncId func, std::vector<NodePtr> args) -> NodePtr;
auto callExprNode(
    const Program& prog,
    sym::FuncId func,
    sym::TypeId resultType,
    std::vector<NodePtr> args,
    CallMode mode) -> NodePtr;

} // namespace prog::expr
