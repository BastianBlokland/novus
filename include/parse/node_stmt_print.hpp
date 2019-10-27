#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"

namespace parse {

class PrintStmtNode final : public Node {
  friend auto printStmtNode(lex::Token kw, NodePtr expr) -> NodePtr;

public:
  PrintStmtNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> lex::SourceSpan override;

private:
  const lex::Token m_kw;
  const NodePtr m_expr;

  PrintStmtNode(lex::Token kw, NodePtr expr);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto printStmtNode(lex::Token kw, NodePtr expr) -> NodePtr;

} // namespace parse
