#pragma once
#include "parse/node_type.hpp"

namespace parse {

class Node {
public:
  Node() = delete;

  [[nodiscard]] auto getType() const noexcept { return m_type; }

protected:
  explicit Node(const NodeType type) : m_type{type} {}

private:
  const NodeType m_type;
};

} // namespace parse
