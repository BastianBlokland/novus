#pragma once
#include "parse/node.hpp"
#include <iterator>
#include <memory>
#include <utility>

namespace parse {

class NodeItrTraits {
public:
  using difference_type   = ptrdiff_t;
  using value_type        = std::unique_ptr<Node>;
  using pointer           = std::unique_ptr<Node>*;
  using reference         = std::unique_ptr<Node>&&;
  using iterator_category = std::input_iterator_tag;
};

template <typename NodeSource>
class NodeItr final : public NodeItrTraits {

  static_assert(
      std::is_same<decltype(std::declval<NodeSource&>().next()), std::unique_ptr<Node>>::value,
      "NodeSource has to have a 'next' function returning a unique_ptr<Node>.");

public:
  NodeItr() : m_source{nullptr}, m_current{nullptr} {}

  explicit NodeItr(NodeSource& nodeSource) : m_source{&nodeSource} {
    // Set the initial value.
    m_current = nodeSource.next();
  }

  auto operator*() -> std::unique_ptr<Node>&& { return std::move(m_current); }

  auto operator-> () -> std::unique_ptr<Node>* { return &m_current; }

  auto operator==(const NodeItr& rhs) noexcept -> bool {
    return m_current.get() == rhs.m_current.get();
  }

  auto operator!=(const NodeItr& rhs) noexcept -> bool {
    return m_current.get() != rhs.m_current.get();
  }

  auto operator++() { m_current = getNode(); }

private:
  NodeSource* m_source;
  std::unique_ptr<Node> m_current;

  auto getNode() -> std::unique_ptr<Node> {
    if (m_source) {
      return m_source->next();
    }
    return nullptr;
  }
};

} // namespace parse
