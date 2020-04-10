#include "internal/prog_rewrite.hpp"
#include "prog/copy.hpp"

namespace opt::internal {

auto rewrite(const prog::Program& prog, const prog::RewriterFactory& rewriterFactory)
    -> prog::Program {
  auto modified = false;
  return rewrite(prog, rewriterFactory, modified);
}

auto rewrite(
    const prog::Program& prog, const prog::RewriterFactory& rewriterFactory, bool& modified)
    -> prog::Program {

  /* Create a new program and copy the types, funcs and execute statements over. For the funcs we
   * rewrite them before copying them over. */

  auto result = prog::Program{};

  // Copy all the types to the new program.
  for (auto typeItr = prog.beginTypeDecls(); typeItr != prog.endTypeDecls(); ++typeItr) {
    prog::copyType(prog, &result, typeItr->first);
  }

  // Rewrite and copy all the functions to the new program.
  for (auto funcItr = prog.beginFuncDecls(); funcItr != prog.endFuncDecls(); ++funcItr) {
    const auto& funcId   = funcItr->first;
    auto funcHasModified = false;
    prog::copyFunc(prog, &result, funcId, funcHasModified, rewriterFactory);

    modified |= funcHasModified;
  }

  // Copy all the execute statements to the new program.
  for (auto execItr = prog.beginExecStmts(); execItr != prog.endExecStmts(); ++execItr) {
    result.addExecStmt(execItr->getConsts(), execItr->getExpr().clone(nullptr));
  }

  return result;
}

} // namespace opt::internal
