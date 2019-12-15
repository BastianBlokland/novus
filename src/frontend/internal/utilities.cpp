#include "utilities.hpp"
#include "frontend/diag_defs.hpp"

namespace frontend::internal {

auto getType(
    const prog::Program& prog, const TypeSubstitutionTable* subTable, const std::string& name)
    -> std::optional<prog::sym::TypeId> {
  if (subTable) {
    const auto subType = subTable->lookupType(name);
    if (subType) {
      return subType;
    }
  }
  return prog.lookupType(name);
}

auto getRetType(
    const Source& src,
    const prog::Program& prog,
    const TypeSubstitutionTable* subTable,
    const parse::FuncDeclStmtNode& n,
    std::vector<Diag>* diags) -> std::optional<prog::sym::TypeId> {

  const auto& retTypeSpec = n.getRetType();
  if (!retTypeSpec) {
    return prog::sym::TypeId::inferType();
  }
  const auto retTypeName = getName(retTypeSpec->getType());
  auto retType           = getType(prog, subTable, retTypeName);
  if (!retType) {
    if (diags) {
      diags->push_back(errUndeclaredType(src, retTypeName, retTypeSpec->getType().getSpan()));
    }
    return std::nullopt;
  }
  return retType;
}

auto getFuncInput(
    const Source& src,
    const prog::Program& prog,
    const TypeSubstitutionTable* subTable,
    const parse::FuncDeclStmtNode& n,
    std::vector<Diag>* diags) -> std::optional<prog::sym::TypeSet> {

  auto isValid  = true;
  auto argTypes = std::vector<prog::sym::TypeId>{};
  for (const auto& arg : n.getArgs()) {
    const auto argTypeName = getName(arg.getType());
    const auto argType     = getType(prog, subTable, argTypeName);
    if (argType) {
      argTypes.push_back(argType.value());
    } else {
      if (diags) {
        diags->push_back(errUndeclaredType(src, argTypeName, arg.getType().getSpan()));
      }
      isValid = false;
    }
  }
  return isValid ? std::optional{prog::sym::TypeSet{std::move(argTypes)}} : std::nullopt;
}

auto getTypeParams(
    const Source& src,
    const prog::Program& prog,
    const parse::TypeParamList& paramList,
    std::vector<Diag>* diags) -> std::optional<std::vector<std::string>> {

  auto typeParams = std::vector<std::string>{};
  auto isValid    = true;
  for (const auto& typeParamToken : paramList) {
    const auto typeParamName = getName(typeParamToken);
    if (prog.lookupType(typeParamName)) {
      if (diags) {
        diags->push_back(
            errTypeParamNameConflictsWithType(src, typeParamName, typeParamToken.getSpan()));
      }
      isValid = false;
    } else {
      typeParams.push_back(typeParamName);
    }
  }
  return isValid ? std::optional(typeParams) : std::nullopt;
}

auto getTypeSet(
    const Source& src,
    const prog::Program& prog,
    const TypeSubstitutionTable* subTable,
    const std::vector<lex::Token>& typeTokens,
    std::vector<Diag>* diags) -> std::optional<prog::sym::TypeSet> {

  auto isValid = true;
  auto types   = std::vector<prog::sym::TypeId>{};
  for (const auto& typeToken : typeTokens) {
    const auto typeName = getName(typeToken);
    auto type           = getType(prog, subTable, typeName);
    if (type) {
      types.push_back(*type);
    } else {
      if (diags) {
        diags->push_back(errUndeclaredType(src, typeName, typeToken.getSpan()));
      }
      isValid = false;
    }
  }
  return isValid ? std::optional{prog::sym::TypeSet{std::move(types)}} : std::nullopt;
}

auto getConstName(
    const Source& src,
    const prog::Program& prog,
    const TypeSubstitutionTable* subTable,
    const prog::sym::ConstDeclTable& consts,
    const lex::Token& nameToken,
    std::vector<Diag>* diags) -> std::optional<std::string> {

  const auto name = getName(nameToken);
  if (subTable != nullptr && subTable->lookupType(name)) {
    if (diags) {
      diags->push_back(errConstNameConflictsWithTypeSubstitution(src, name, nameToken.getSpan()));
    }
    return std::nullopt;
  }
  if (prog.lookupType(name)) {
    if (diags) {
      diags->push_back(errConstNameConflictsWithType(src, name, nameToken.getSpan()));
    }
    return std::nullopt;
  }
  if (!prog.lookupFuncs(name).empty()) {
    if (diags) {
      diags->push_back(errConstNameConflictsWithFunction(src, name, nameToken.getSpan()));
    }
    return std::nullopt;
  }
  if (!prog.lookupActions(name).empty()) {
    if (diags) {
      diags->push_back(errConstNameConflictsWithAction(src, name, nameToken.getSpan()));
    }
    return std::nullopt;
  }
  if (consts.lookup(name)) {
    if (diags) {
      diags->push_back(errConstNameConflictsWithConst(src, name, nameToken.getSpan()));
    }
    return std::nullopt;
  }
  return name;
}

} // namespace frontend::internal
