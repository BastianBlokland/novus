#pragma once
#include "lex/token.hpp"
#include "parse/argument_list_decl.hpp"
#include "parse/node.hpp"

namespace parse {

class AnonFuncExprNode final : public Node {
public:
  friend auto anonFuncExprNode(lex::Token kw, ArgumentListDecl argList, NodePtr body) -> NodePtr;

  AnonFuncExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  [[nodiscard]] auto getArgList() const -> const ArgumentListDecl&;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const lex::Token m_kw;
  const ArgumentListDecl m_argList;
  const NodePtr m_body;

  AnonFuncExprNode(lex::Token kw, ArgumentListDecl argList, NodePtr body);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto anonFuncExprNode(lex::Token kw, ArgumentListDecl argList, NodePtr body) -> NodePtr;

} // namespace parse
