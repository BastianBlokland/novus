#pragma once
#include "input/span.hpp"
#include "node_visitor.hpp"
#include <iostream>

namespace parse {

class Node {
  friend auto operator<<(std::ostream& out, const Node& rhs) -> std::ostream&;

public:
  Node(const Node& rhs)     = delete;
  Node(Node&& rhs) noexcept = delete;
  virtual ~Node()           = default;

  auto operator=(const Node& rhs) -> Node& = delete;
  auto operator=(Node&& rhs) noexcept -> Node& = delete;

  virtual auto operator==(const Node& rhs) const noexcept -> bool = 0;
  virtual auto operator!=(const Node& rhs) const noexcept -> bool = 0;

  [[nodiscard]] virtual auto operator[](int) const -> const Node&  = 0;
  [[nodiscard]] virtual auto getChildCount() const -> unsigned int = 0;
  [[nodiscard]] virtual auto getSpan() const -> input::Span        = 0;

  virtual auto accept(NodeVisitor* visitor) const -> void = 0;

protected:
  Node() = default;

  virtual auto print(std::ostream& out) const -> std::ostream& = 0;
};

using NodePtr = std::unique_ptr<Node>;

auto operator<<(std::ostream& out, const Node& rhs) -> std::ostream&;

} // namespace parse
