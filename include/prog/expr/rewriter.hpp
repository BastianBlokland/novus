#pragma once
#include "prog/expr/node.hpp"

namespace prog::expr {

class Rewriter {
public:
  [[nodiscard]] virtual auto rewrite(const Node& expr) -> NodePtr = 0;
};

} // namespace prog::expr
