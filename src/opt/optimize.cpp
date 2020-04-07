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

  // Precompute computations where all arguments are literals.
  result = precomputeLiterals(result);

  // Remove any constants where its cheaper just to 'inline' the expression.
  result = eliminateConsts(result);

  // Run another precompute pass as the 'eliminateConsts' pass could have rewritten constants into
  // literals.
  // TODO: Consider if we need to keep running 'eliminateConsts' and 'precomputeLiterals' in a loop
  // until no more rewrites are done.
  result = precomputeLiterals(result);

  return result;
}

} // namespace opt
