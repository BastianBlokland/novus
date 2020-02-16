#pragma once
#include "lex/token.hpp"
#include "parse/argument_list_decl.hpp"
#include "parse/node.hpp"
#include <vector>

namespace parse {

class AnonFuncExprNode final : public Node {
public:
  friend auto anonFuncExprNode(
      std::vector<lex::Token> modifiers, lex::Token kw, ArgumentListDecl argList, NodePtr body)
      -> NodePtr;

  AnonFuncExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  [[nodiscard]] auto isImpure() const -> bool;
  [[nodiscard]] auto getArgList() const -> const ArgumentListDecl&;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const std::vector<lex::Token> m_modifiers;
  const lex::Token m_kw;
  const ArgumentListDecl m_argList;
  const NodePtr m_body;

  AnonFuncExprNode(
      std::vector<lex::Token> modifiers, lex::Token kw, ArgumentListDecl argList, NodePtr body);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto anonFuncExprNode(
    std::vector<lex::Token> modifiers, lex::Token kw, ArgumentListDecl argList, NodePtr body)
    -> NodePtr;

} // namespace parse
