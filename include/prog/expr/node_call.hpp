#pragma once
#include "prog/expr/call_mode.hpp"
#include "prog/expr/node.hpp"
#include "prog/program.hpp"

namespace prog::expr {

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

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  sym::FuncId m_func;
  sym::TypeId m_resultType;
  std::vector<NodePtr> m_args;
  CallMode m_mode;

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
