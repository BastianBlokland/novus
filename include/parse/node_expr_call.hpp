#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include <vector>

namespace parse {

class CallExprNode final : public Node {
public:
  CallExprNode() = delete;

  CallExprNode(
      lex::Token id,
      lex::Token open,
      std::vector<NodePtr> args,
      std::vector<lex::Token> commas,
      lex::Token close);

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getId() const -> const lex::Token&;

private:
  const lex::Token m_id;
  const lex::Token m_open;
  const std::vector<NodePtr> m_args;
  const std::vector<lex::Token> m_commas;
  const lex::Token m_close;

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto callExprNode(
    lex::Token id,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) -> NodePtr;

} // namespace parse
