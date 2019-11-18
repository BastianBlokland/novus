#include "frontend/analysis.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/declare_user_funcs.hpp"
#include "internal/define_exec_stmts.hpp"
#include "internal/define_user_funcs.hpp"
#include "internal/get_parse_diags.hpp"
#include "internal/typeinfer_user_funcs.hpp"
#include "parse/nodes.hpp"
#include "prog/program.hpp"
#include <memory>
#include <vector>

namespace frontend {

auto analyze(const Source& src) -> Output {
  // Check source for any parse errors.
  auto getParseDiags = internal::GetParseDiags{src};
  src.accept(&getParseDiags);
  if (getParseDiags.hasErrors()) {
    return buildOutput(nullptr, getParseDiags.getDiags());
  }

  auto prog = std::make_unique<prog::Program>();

  // Declare user functions.
  auto declareUserFuncs = internal::DeclareUserFuncs{src, prog.get()};
  src.accept(&declareUserFuncs);
  if (declareUserFuncs.hasErrors()) {
    return buildOutput(nullptr, declareUserFuncs.getDiags());
  }

  // Infer return-types of user functions (run multiple passes until all have been inferred).
  auto typeInferUserFuncs = internal::TypeInferUserFuncs{prog.get()};
  bool inferredAllFuncs;
  do {
    inferredAllFuncs = true;
    auto inferredOne = false;
    for (const auto& funcDeclInfo : declareUserFuncs.getFuncs()) {
      auto& funcDecl = prog->getFuncDecl(funcDeclInfo.first);
      if (funcDecl.getSig().getOutput().isConcrete()) {
        continue;
      }
      auto success = typeInferUserFuncs.inferRetType(funcDeclInfo.first, funcDeclInfo.second);
      inferredAllFuncs &= success;
      inferredOne |= success;
    }
    if (!inferredOne) {
      break;
    }
  } while (!inferredAllFuncs);
  if (!inferredAllFuncs) {
    auto diags = std::vector<Diag>();
    for (const auto& funcDeclInfo : declareUserFuncs.getFuncs()) {
      auto& funcDecl = prog->getFuncDecl(funcDeclInfo.first);
      if (funcDecl.getSig().getOutput().isInfer()) {
        diags.push_back(errUnableToInferFuncReturnType(
            src, funcDecl.getName(), funcDeclInfo.second[0].getSpan()));
      }
    }
    return buildOutput(nullptr, std::move(diags));
  }

  // Define user functions.
  auto defineUserFuncs = internal::DefineUserFuncs{src, prog.get()};
  for (const auto& funcDecl : declareUserFuncs.getFuncs()) {
    defineUserFuncs.define(funcDecl.first, funcDecl.second);
  }
  if (defineUserFuncs.hasErrors()) {
    return buildOutput(nullptr, defineUserFuncs.getDiags());
  }

  // Define execute statements.
  auto defineExecStmts = internal::DefineExecStmts{src, prog.get()};
  src.accept(&defineExecStmts);
  if (defineExecStmts.hasErrors()) {
    return buildOutput(nullptr, defineExecStmts.getDiags());
  }

  return buildOutput(std::move(prog), {});
}

} // namespace frontend
