#include "utilities.hpp"
#include <algorithm>

namespace prog::expr {

auto anyNodeNull(const std::vector<NodePtr>& v) -> bool {
  return std::any_of(v.begin(), v.end(), [](const NodePtr& p) { return p == nullptr; });
}

auto nodesEqual(const std::vector<NodePtr>& a, const std::vector<NodePtr>& b) -> bool {
  return a.size() == b.size() &&
      std::equal(a.begin(), a.end(), b.begin(), [](const NodePtr& l, const NodePtr& r) {
           return *l == *r;
         });
}

auto getType(const std::vector<NodePtr>& nodes) -> std::optional<sym::TypeId> {
  if (nodes.empty()) {
    return std::nullopt;
  }
  const auto type = (*nodes.begin())->getType();
  for (const auto& node : nodes) {
    if (node->getType() != type) {
      return std::nullopt;
    }
  }
  return type;
}

} // namespace prog::expr
