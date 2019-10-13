#pragma once
#include "parse/node_type.hpp"

namespace parse {

class Node {
public:
  Node()                    = delete;
  Node(const Node& rhs)     = default;
  Node(Node&& rhs) noexcept = default;
  virtual ~Node()           = default;

  auto operator=(const Node& rhs) -> Node& = delete;
  auto operator=(Node&& rhs) noexcept -> Node& = delete;

  virtual auto operator==(const Node* rhs) const noexcept -> bool = 0;
  virtual auto operator!=(const Node* rhs) const noexcept -> bool = 0;

  [[nodiscard]] virtual auto clone() -> std::unique_ptr<Node> = 0;

  [[nodiscard]] auto getType() const noexcept { return m_type; }

protected:
  explicit Node(const NodeType type) : m_type{type} {}

private:
  const NodeType m_type;
};

} // namespace parse
