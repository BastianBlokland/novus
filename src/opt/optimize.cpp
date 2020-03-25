#include "opt/opt.hpp"

namespace opt {

auto optimize(const prog::Program& prog) -> prog::Program {
  // Remove any unused types and functions.
  // We start with one pass of treeshaking to avoid inlining unused functions.
  auto result = treeshake(prog);

  // Inline possible functions.
  result = inlineCalls(result);

  // Remove any functions that have become unused due to inlining.
  result = treeshake(result);

  // Remove any constants where its cheaper just to 'inline' the expression.
  result = eliminateConsts(result);

  return result;
}

} // namespace opt
