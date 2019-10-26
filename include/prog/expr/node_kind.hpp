#pragma once
#include <iostream>

namespace prog::expr {

enum class NodeKind {
  Group,
  Branch,
  Assign,
  Const,
  Call,
  LitInt,
  LitBool,
};

auto operator<<(std::ostream& out, const NodeKind& rhs) -> std::ostream&;

} // namespace prog::expr
