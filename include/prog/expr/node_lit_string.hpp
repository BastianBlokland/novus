#pragma once
#include "prog/expr/node.hpp"
#include "prog/program.hpp"

namespace prog::expr {

class LitStringNode final : public Node {
  friend auto litStringNode(const Program& program, std::string val) -> NodePtr;

public:
  LitStringNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;
  [[nodiscard]] auto toString() const -> std::string override;

  [[nodiscard]] auto clone() const -> std::unique_ptr<Node> override;

  [[nodiscard]] auto getVal() const noexcept -> const std::string&;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  sym::TypeId m_type;
  std::string m_val;

  LitStringNode(sym::TypeId type, std::string val);
};

// Factories.
auto litStringNode(const Program& program, std::string val) -> NodePtr;

} // namespace prog::expr
