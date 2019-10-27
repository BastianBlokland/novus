#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include <vector>

namespace parse {

class GroupExprNode final : public Node {
  friend auto groupExprNode(std::vector<NodePtr> subExprs, std::vector<lex::Token> semis)
      -> NodePtr;

public:
  GroupExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> lex::SourceSpan override;

private:
  const std::vector<NodePtr> m_subExprs;
  const std::vector<lex::Token> m_semis;

  GroupExprNode(std::vector<NodePtr> subExprs, std::vector<lex::Token> semis);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto groupExprNode(std::vector<NodePtr> subExprs, std::vector<lex::Token> semis) -> NodePtr;

} // namespace parse
