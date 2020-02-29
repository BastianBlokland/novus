#pragma once
#include "prog/expr/node.hpp"
#include "prog/program.hpp"

namespace prog::expr {

class LitFuncNode final : public Node {
  friend auto litFuncNode(const Program& program, sym::TypeId type, sym::FuncId func) -> NodePtr;

public:
  LitFuncNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;
  [[nodiscard]] auto toString() const -> std::string override;

  [[nodiscard]] auto clone() const -> std::unique_ptr<Node> override;

  [[nodiscard]] auto getFunc() const noexcept -> sym::FuncId;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  sym::TypeId m_type;
  sym::FuncId m_func;

  LitFuncNode(sym::TypeId type, sym::FuncId func);
};

// Factories.
auto litFuncNode(const Program& program, sym::TypeId type, sym::FuncId func) -> NodePtr;

} // namespace prog::expr
