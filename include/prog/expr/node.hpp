#pragma once
#include "prog/sym/type_id.hpp"
#include <memory>

namespace prog::expr {

class Node {
public:
  Node(const Node& rhs)     = delete;
  Node(Node&& rhs) noexcept = delete;
  virtual ~Node()           = default;

  auto operator=(const Node& rhs) -> Node& = delete;
  auto operator=(Node&& rhs) noexcept -> Node& = delete;

  virtual auto operator==(const Node& rhs) const noexcept -> bool = 0;
  virtual auto operator!=(const Node& rhs) const noexcept -> bool = 0;

  [[nodiscard]] virtual auto getType() const noexcept -> sym::TypeId = 0;

protected:
  Node() = default;
};

using NodePtr = std::unique_ptr<Node>;

} // namespace prog::expr
