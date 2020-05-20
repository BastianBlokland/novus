#pragma once
#include "prog/expr/node.hpp"

namespace prog::expr {

// Optionally rewrite a node in the tree.
class Rewriter {
public:
  Rewriter()                        = default;
  Rewriter(const Rewriter& rhs)     = delete;
  Rewriter(Rewriter&& rhs) noexcept = delete;
  virtual ~Rewriter()               = default;

  auto operator=(const Rewriter& rhs) -> Rewriter& = delete;
  auto operator=(Rewriter&& rhs) noexcept -> Rewriter& = delete;

  [[nodiscard]] virtual auto rewrite(const Node& expr) -> NodePtr = 0;
  [[nodiscard]] virtual auto hasModified() -> bool                = 0;
};

} // namespace prog::expr
