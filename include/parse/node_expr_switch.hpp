#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include <vector>

namespace parse {

class SwitchExprNode final : public Node {
public:
  SwitchExprNode() = delete;
  SwitchExprNode(std::vector<NodePtr> ifClauses, NodePtr elseClause);

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;

private:
  const std::vector<NodePtr> m_ifClauses;
  const NodePtr m_elseClause;

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto switchExprNode(std::vector<NodePtr> ifClauses, NodePtr elseClause) -> NodePtr;

} // namespace parse
