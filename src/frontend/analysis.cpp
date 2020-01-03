#include "frontend/output.hpp"
#include "frontend/source.hpp"
#include "internal/context.hpp"
#include "internal/declare_user_funcs.hpp"
#include "internal/declare_user_types.hpp"
#include "internal/define_exec_stmts.hpp"
#include "internal/define_user_funcs.hpp"
#include "internal/define_user_types.hpp"
#include "internal/func_template_table.hpp"
#include "internal/get_parse_diags.hpp"
#include "internal/type_template_table.hpp"
#include "internal/typeinfer_user_funcs.hpp"
#include "internal/validate_types.hpp"
#include <memory>
#include <vector>

namespace frontend {

auto analyze(const Source& src) -> Output {
  auto prog          = std::make_unique<prog::Program>();
  auto typeTemplates = internal::TypeTemplateTable{};
  auto funcTemplates = internal::FuncTemplateTable{};
  auto delegates     = internal::DelegateTable{};
  auto context = internal::Context{src, prog.get(), &typeTemplates, &funcTemplates, &delegates};

  // Check source for any parse errors.
  auto getParseDiags = internal::GetParseDiags{&context};
  src.accept(&getParseDiags);
  if (context.hasErrors()) {
    return buildOutput(nullptr, context.getDiags());
  }

  // Declare user types.
  auto declareUserTypes = internal::DeclareUserTypes{&context};
  src.accept(&declareUserTypes);
  if (context.hasErrors()) {
    return buildOutput(nullptr, context.getDiags());
  }

  // Define user types.
  auto defineUserTypes = internal::DefineUserTypes{&context, nullptr};
  for (const auto& structDecl : declareUserTypes.getStructs()) {
    defineUserTypes.define(structDecl.first, structDecl.second);
  }
  for (const auto& unionDecl : declareUserTypes.getUnions()) {
    defineUserTypes.define(unionDecl.first, unionDecl.second);
  }
  if (context.hasErrors()) {
    return buildOutput(nullptr, context.getDiags());
  }

  // Declare user functions.
  auto declareUserFuncs = internal::DeclareUserFuncs{&context};
  src.accept(&declareUserFuncs);
  if (context.hasErrors()) {
    return buildOutput(nullptr, context.getDiags());
  }

  // Infer return-types of user functions (run multiple passes until all have been inferred).
  auto typeInferUserFuncs = internal::TypeInferUserFuncs{&context, nullptr};
  auto firstInferItr      = true;
  bool inferredAllFuncs;
  do {
    inferredAllFuncs = true;
    auto inferredOne = false;
    for (const auto& funcDeclInfo : declareUserFuncs.getFuncs()) {
      auto& funcDecl = prog->getFuncDecl(funcDeclInfo.first);
      if (funcDecl.getOutput().isConcrete()) {
        continue;
      }
      auto success = typeInferUserFuncs.inferRetType(funcDeclInfo.first, funcDeclInfo.second);
      if (context.hasErrors()) {
        return buildOutput(nullptr, context.getDiags());
      }
      inferredAllFuncs &= success;
      inferredOne |= success;
    }
    // Stop the loop if we failed to make any progress.
    if (!firstInferItr && !inferredOne) {
      break;
    }
    firstInferItr = false;
  } while (!inferredAllFuncs);

  // Define user functions.
  auto defineUserFuncs = internal::DefineUserFuncs{&context, nullptr};
  for (const auto& funcDecl : declareUserFuncs.getFuncs()) {
    if (!context.hasErrors()) {
      defineUserFuncs.define(funcDecl.first, funcDecl.second);
    }
  }
  if (context.hasErrors()) {
    return buildOutput(nullptr, context.getDiags());
  }

  // Define execute statements.
  auto defineExecStmts = internal::DefineExecStmts{&context};
  src.accept(&defineExecStmts);
  if (context.hasErrors()) {
    return buildOutput(nullptr, context.getDiags());
  }

  // Validate the type-definitions (used to detect things like cyclic structs).
  auto validateTypes = internal::ValidateTypes{&context};
  for (auto typeItr = prog->beginTypeDecls(); typeItr != prog->endTypeDecls(); ++typeItr) {
    validateTypes.validate(typeItr->getId());
  }
  if (context.hasErrors()) {
    return buildOutput(nullptr, context.getDiags());
  }

  return buildOutput(std::move(prog), {});
}

} // namespace frontend
