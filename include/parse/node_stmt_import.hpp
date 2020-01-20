#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"

namespace parse {

class ImportStmtNode final : public Node {
  friend auto importStmtNode(lex::Token kw, lex::Token path) -> NodePtr;

public:
  ImportStmtNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  [[nodiscard]] auto getPath() const -> const lex::Token&;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const lex::Token m_kw;
  const lex::Token m_path;

  ImportStmtNode(lex::Token kw, lex::Token path);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto importStmtNode(lex::Token kw, lex::Token path) -> NodePtr;

} // namespace parse
