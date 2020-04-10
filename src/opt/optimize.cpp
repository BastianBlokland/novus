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

  // Keep eliminating constants and precomputing literals until program cannot be simplified
  // anymore.
  bool modified;
  do {
    modified = false;

    // Remove any constants where its cheaper just to 'inline' the expression.
    result = eliminateConsts(result, modified);

    // Precompute computations where all arguments are literals.
    result = precomputeLiterals(result, modified);

  } while (modified);

  return result;
}

} // namespace opt
