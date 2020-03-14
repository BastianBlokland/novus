#pragma once
#include "prog/expr/node.hpp"
#include <optional>
#include <vector>

namespace prog::expr {

[[nodiscard]] auto anyNodeNull(const std::vector<NodePtr>& v) -> bool;

[[nodiscard]] auto nodesEqual(const std::vector<NodePtr>& a, const std::vector<NodePtr>& b) -> bool;

[[nodiscard]] auto getType(const std::vector<NodePtr>& nodes) -> std::optional<sym::TypeId>;

[[nodiscard]] auto cloneNodes(const std::vector<NodePtr>& nodes, Rewriter* rewriter)
    -> std::vector<NodePtr>;

} // namespace prog::expr
