#include "opt/opt.hpp"

namespace opt {

auto optimize(const prog::Program& prog) -> prog::Program {
  // Remove any unused types and functions.
  // We start with one pass of treeshaking to avoid inlining unused functions.
  auto result = treeshake(prog);

  // Run a single pass of const elimination and literal precompute before inlining. Reason is
  // dynamic calls to function literals are simplified to be static calls (which we can then inlined
  // in the next phase).
  result = eliminateConsts(result);
  result = precomputeLiterals(result);

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
