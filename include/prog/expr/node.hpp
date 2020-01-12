#pragma once
#include "prog/expr/node_visitor.hpp"
#include "prog/sym/type_id.hpp"
#include <iostream>
#include <memory>
#include <string>

namespace prog::expr {

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

  [[nodiscard]] virtual auto operator[](unsigned int) const -> const Node& = 0;
  [[nodiscard]] virtual auto getChildCount() const -> unsigned int         = 0;
  [[nodiscard]] virtual auto getType() const noexcept -> sym::TypeId       = 0;
  [[nodiscard]] virtual auto toString() const -> std::string               = 0;

  virtual auto accept(NodeVisitor* visitor) const -> void = 0;

protected:
  Node() = default;
};

using NodePtr = std::unique_ptr<Node>;

auto operator<<(std::ostream& out, const Node& rhs) -> std::ostream&;

} // namespace prog::expr
