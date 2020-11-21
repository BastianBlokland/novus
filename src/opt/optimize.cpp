#include "opt/opt.hpp"

namespace opt {

auto optimize(const prog::Program& prog) -> prog::Program {

  // We start with one pass of treeshaking to avoid optimizing unused functions.
  auto result = treeshake(prog);

  // Keep optimizing until the program cannot be simplified anymore.
  bool modified;
  do {
    modified = false;

    // Remove any constants where its cheaper just to 'inline' the expression.
    result = eliminateConsts(result, modified);

    // Precompute computations where all arguments are literals.
    result = precomputeLiterals(result, modified);

    // Inline possible functions.
    result = inlineCalls(result, modified);

  } while (modified);

  // Remove any functions that have become unused due to inlining.
  result = treeshake(result);

  return result;
}

} // namespace opt
