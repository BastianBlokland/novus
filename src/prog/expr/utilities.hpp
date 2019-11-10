#pragma once
#include "prog/expr/node.hpp"
#include <optional>
#include <vector>

namespace prog::expr {

auto anyNodeNull(const std::vector<NodePtr>& v) -> bool;

auto nodesEqual(const std::vector<NodePtr>& a, const std::vector<NodePtr>& b) -> bool;

auto getType(const std::vector<NodePtr>& nodes) -> std::optional<sym::TypeId>;

} // namespace prog::expr
