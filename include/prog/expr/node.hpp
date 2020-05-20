#pragma once
#include "prog/expr/node_kind.hpp"
#include "prog/expr/node_visitor.hpp"
#include "prog/sym/type_id.hpp"
#include <iostream>
#include <memory>
#include <string>

namespace prog::expr {

class Rewriter;

// Base-class for a node in the tree.
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

  [[nodiscard]] auto getKind() const -> NodeKind;

  [[nodiscard]] virtual auto operator[](unsigned int) const -> const Node& = 0;
  [[nodiscard]] virtual auto getChildCount() const -> unsigned int         = 0;
  [[nodiscard]] virtual auto getType() const noexcept -> sym::TypeId       = 0;
  [[nodiscard]] virtual auto toString() const -> std::string               = 0;

  [[nodiscard]] virtual auto clone(Rewriter* rewriter) const -> std::unique_ptr<Node> = 0;

  virtual auto accept(NodeVisitor* visitor) const -> void = 0;

  // Downcast to a node implementation, throws if node is not of the child type.
  template <typename NodeType>
  [[nodiscard]] auto downcast() const -> const NodeType* {
    if (getKind() != NodeType::getKind()) {
      throw std::logic_error{"Node is of incorrect type"};
    }
    return static_cast<const NodeType*>(this); // NOLINT: Downcast, we've checked it safe.
  }

protected:
  explicit Node(NodeKind kind);

private:
  NodeKind m_kind;
};

using NodePtr = std::unique_ptr<Node>;

auto operator<<(std::ostream& out, const Node& rhs) -> std::ostream&;

} // namespace prog::expr
