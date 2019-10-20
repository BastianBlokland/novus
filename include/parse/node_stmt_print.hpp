#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"

namespace parse {

class PrintStmtNode final : public Node {
public:
  PrintStmtNode() = delete;
  PrintStmtNode(lex::Token kw, NodePtr expr);

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> int override;

private:
  const lex::Token m_kw;
  const NodePtr m_expr;

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto printStmtNode(lex::Token kw, NodePtr expr) -> NodePtr;

} // namespace parse
