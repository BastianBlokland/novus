#pragma once
#include "parse/node.hpp"

namespace parse {

class ExecStmtNode final : public Node {
  friend auto execStmtNode(NodePtr callExpr) -> NodePtr;

public:
  ExecStmtNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const NodePtr m_callExpr;

  explicit ExecStmtNode(NodePtr callExpr);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto execStmtNode(NodePtr callExpr) -> NodePtr;

} // namespace parse
