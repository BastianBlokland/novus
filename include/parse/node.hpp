#pragma once
#include "lex/source_span.hpp"
#include "parse/node_kind.hpp"
#include <iostream>

namespace parse {

class Node {
public:
  Node()                    = delete;
  Node(const Node& rhs)     = delete;
  Node(Node&& rhs) noexcept = delete;
  virtual ~Node()           = default;

  auto operator=(const Node& rhs) -> Node& = delete;
  auto operator=(Node&& rhs) noexcept -> Node& = delete;

  virtual auto operator==(const Node& rhs) const noexcept -> bool = 0;
  virtual auto operator!=(const Node& rhs) const noexcept -> bool = 0;

  [[nodiscard]] virtual auto operator[](int) const -> const Node& = 0;
  [[nodiscard]] auto getType() const noexcept -> NodeKind { return m_type; }
  [[nodiscard]] virtual auto getChildCount() const -> unsigned int = 0;
  [[nodiscard]] virtual auto getSpan() const -> lex::SourceSpan    = 0;

  virtual auto print(std::ostream& out) const -> std::ostream& = 0;

protected:
  explicit Node(const NodeKind type) : m_type{type} {}

private:
  const NodeKind m_type;
};

using NodePtr = std::unique_ptr<Node>;

auto operator<<(std::ostream& out, const Node& rhs) -> std::ostream&;

} // namespace parse
