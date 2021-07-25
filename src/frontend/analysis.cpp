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
#include "internal/import_sources.hpp"
#include "internal/patch_args.hpp"
#include "internal/type_info.hpp"
#include "internal/type_template_table.hpp"
#include "internal/typeinfer_user_funcs.hpp"
#include "internal/validate_types.hpp"
#include <forward_list>
#include <memory>
#include <vector>

namespace frontend {

auto buildOutput(
    std::unique_ptr<prog::Program> prog,
    std::forward_list<Source> importedSources,
    SourceTable sourceTable,
    std::vector<Diag> diags) -> Output;

using TypeInfoMap =
    std::unordered_map<prog::sym::TypeId, internal::TypeInfo, prog::sym::TypeIdHasher>;

template <typename VisitorType, class... VisitorArgs>
auto visitAll(std::vector<internal::Context>* ctxs, VisitorArgs&&... visitorArgs) {
  for (auto& ctx : *ctxs) {
    auto visitor = VisitorType{&ctx, std::forward<VisitorArgs>(visitorArgs)...};
    ctx.getSrc().accept(&visitor);
  }
}

auto analyze(const Source& mainSrc, const std::vector<filesystem::path>& searchPaths) -> Output {
  auto prog               = std::make_unique<prog::Program>();
  auto sourceTableBuilder = internal::SourceTableBuilder{};
  auto typeTemplates      = internal::TypeTemplateTable{};
  auto funcTemplates      = internal::FuncTemplateTable{};
  auto delegates          = internal::DelegateTable{};
  auto futures            = internal::FutureTable{};
  auto lazies             = internal::LazyTable{};
  auto fails              = internal::FailTable{};
  auto staticIntTable     = internal::StaticIntTable{};
  auto typeInfos          = TypeInfoMap{};
  auto diags              = std::vector<Diag>{};
  auto makeCtx            = [&](const Source& src) {
    return internal::Context(
        src,
        prog.get(),
        &sourceTableBuilder,
        &typeTemplates,
        &funcTemplates,
        &delegates,
        &futures,
        &lazies,
        &fails,
        &staticIntTable,
        &typeInfos,
        &diags);
  };

  // Resolve all imports.
  auto importedSources = std::forward_list<Source>{};
  auto import =
      internal::ImportSources{mainSrc, searchPaths, &importedSources, &sourceTableBuilder, &diags};
  mainSrc.accept(&import);
  auto allContexts = std::vector<internal::Context>{makeCtx(mainSrc)};
  for (const auto& importSrc : importedSources) {
    allContexts.push_back(makeCtx(importSrc));
  }

  // Check any parse errors.
  visitAll<internal::GetParseDiags>(&allContexts);
  if (!diags.empty()) {
    return buildOutput(nullptr, std::move(importedSources), sourceTableBuilder.build(), diags);
  }

  // Declare user types.
  auto structDeclInfos = std::vector<internal::StructDeclInfo>{};
  auto unionDeclInfos  = std::vector<internal::UnionDeclInfo>{};
  auto enumDeclInfos   = std::vector<internal::EnumDeclInfo>{};
  visitAll<internal::DeclareUserTypes>(
      &allContexts, &structDeclInfos, &unionDeclInfos, &enumDeclInfos);
  if (!diags.empty()) {
    return buildOutput(nullptr, std::move(importedSources), sourceTableBuilder.build(), diags);
  }

  // Define user types.
  for (const auto& sDeclInfo : structDeclInfos) {
    defineType(sDeclInfo.getCtx(), sDeclInfo.getId(), sDeclInfo.getParseNode());
  }
  for (const auto& uDeclInfo : unionDeclInfos) {
    defineType(uDeclInfo.getCtx(), uDeclInfo.getId(), uDeclInfo.getParseNode());
  }
  for (const auto& eDeclInfo : enumDeclInfos) {
    defineType(eDeclInfo.getCtx(), eDeclInfo.getId(), eDeclInfo.getParseNode());
  }
  if (!diags.empty()) {
    return buildOutput(nullptr, std::move(importedSources), sourceTableBuilder.build(), diags);
  }

  // Declare user functions.
  auto funcDeclInfos = std::vector<internal::FuncDeclInfo>{};
  visitAll<internal::DeclareUserFuncs>(&allContexts, &funcDeclInfos);
  if (!diags.empty()) {
    return buildOutput(nullptr, std::move(importedSources), sourceTableBuilder.build(), diags);
  }

  // Infer return-types of user functions (run multiple passes until all have been inferred).
  auto typeInferUserFuncs = internal::TypeInferUserFuncs{nullptr};
  auto firstInferItr      = true;
  bool inferredAllFuncs;
  do {
    inferredAllFuncs = true;
    auto inferredOne = false;
    for (const auto& fDeclInfo : funcDeclInfos) {
      auto& funcDecl = prog->getFuncDecl(fDeclInfo.getId());
      if (funcDecl.getOutput().isConcrete()) {
        continue;
      }
      auto success = typeInferUserFuncs.inferRetType(
          fDeclInfo.getCtx(), fDeclInfo.getId(), fDeclInfo.getParseNode());
      if (!diags.empty()) {
        return buildOutput(nullptr, std::move(importedSources), sourceTableBuilder.build(), diags);
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
  for (const auto& fDeclInfo : funcDeclInfos) {
    internal::defineFunc(
        fDeclInfo.getCtx(),
        nullptr,
        fDeclInfo.getId(),
        fDeclInfo.getDisplayName(),
        fDeclInfo.getParseNode());
  }
  if (!diags.empty()) {
    return buildOutput(nullptr, std::move(importedSources), sourceTableBuilder.build(), diags);
  }

  // Define execute statements from the main-source.
  auto defineExecStmts = internal::DefineExecStmts{allContexts.data()};
  mainSrc.accept(&defineExecStmts);
  if (!diags.empty()) {
    return buildOutput(nullptr, std::move(importedSources), sourceTableBuilder.build(), diags);
  }

  // Validate the type-definitions (used to detect things like cyclic structs).
  for (const auto& t : typeInfos) {
    validateType(t.first, t.second);
  }
  if (!diags.empty()) {
    return buildOutput(nullptr, std::move(importedSources), sourceTableBuilder.build(), diags);
  }

  // Create a table for looking up source information about expressions in the result program.
  auto sourceTable = sourceTableBuilder.build();

  // Patch all call expressions to apply the optional arguments if they didn't supply overrides.
  internal::patchCallArgs(*prog, sourceTable, [&diags](Diag diag) { diags.push_back(diag); });

  if (!diags.empty()) {
    return buildOutput(nullptr, std::move(importedSources), std::move(sourceTable), diags);
  }

  return buildOutput(std::move(prog), std::move(importedSources), std::move(sourceTable), {});
}

auto getDependencies(const Source& mainSrc, const std::vector<filesystem::path>& searchPaths)
    -> std::forward_list<Source> {
  auto dependencies = std::forward_list<Source>{};
  auto import       = internal::ImportSources{mainSrc, searchPaths, &dependencies};
  mainSrc.accept(&import);
  return dependencies;
}

} // namespace frontend
