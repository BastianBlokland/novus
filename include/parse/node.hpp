#pragma once
#include "parse/node_type.hpp"
#include <iostream>

namespace parse {

class Node {
public:
  Node()                    = delete;
  Node(const Node& rhs)     = default;
  Node(Node&& rhs) noexcept = default;
  virtual ~Node()           = default;

  auto operator=(const Node& rhs) -> Node& = delete;
  auto operator=(Node&& rhs) noexcept -> Node& = delete;

  virtual auto operator==(const Node& rhs) const noexcept -> bool = 0;
  virtual auto operator!=(const Node& rhs) const noexcept -> bool = 0;

  [[nodiscard]] virtual auto operator[](int) const -> const Node& = 0;

  [[nodiscard]] auto getType() const noexcept -> NodeType { return m_type; }

  [[nodiscard]] virtual auto getChildCount() const -> int = 0;

  [[nodiscard]] virtual auto clone() const -> std::unique_ptr<Node> = 0;

  virtual auto print(std::ostream& out) const -> std::ostream& = 0;

protected:
  explicit Node(const NodeType type) : m_type{type} {}

private:
  const NodeType m_type;
};

using NodePtr = std::unique_ptr<Node>;

auto operator<<(std::ostream& out, const Node& rhs) -> std::ostream&;

} // namespace parse
