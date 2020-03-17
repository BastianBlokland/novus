#pragma once
#include "prog/expr/node.hpp"
#include "prog/program.hpp"

namespace prog::expr {

class LitEnumNode final : public Node {
  friend auto litEnumNode(const Program& program, sym::TypeId enumType, std::string name)
      -> NodePtr;

public:
  LitEnumNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] constexpr static auto getKind() { return NodeKind::LitEnum; }

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;
  [[nodiscard]] auto toString() const -> std::string override;

  [[nodiscard]] auto clone(Rewriter* rewriter) const -> std::unique_ptr<Node> override;

  [[nodiscard]] auto getName() const noexcept -> const std::string&;
  [[nodiscard]] auto getVal() const noexcept -> int32_t;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  sym::TypeId m_type;
  std::string m_name;
  int32_t m_val;

  LitEnumNode(sym::TypeId type, std::string entryName, int32_t val);
};

// Factories.
auto litEnumNode(const Program& prog, sym::TypeId enumType, std::string name) -> NodePtr;

} // namespace prog::expr
