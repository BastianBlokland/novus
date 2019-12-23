#include "utilities.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/context.hpp"
#include "internal/typeinfer_expr.hpp"
#include "parse/type_param_list.hpp"
#include <stdexcept>

namespace frontend::internal {

auto getOrInstType(
    Context* context, const TypeSubstitutionTable* subTable, const parse::Type& parseType)
    -> std::optional<prog::sym::TypeId> {

  const auto name = getName(parseType.getId());
  if (subTable) {
    const auto subType = subTable->lookupType(name);
    if (subType) {
      if (parseType.getParamList()) {
        context->reportDiag(
            errTypeParamOnSubstitutionType(context->getSrc(), name, parseType.getSpan()));
        return std::nullopt;
      }
      return subType;
    }
  }
  auto paramList = parseType.getParamList();
  if (paramList) {
    const auto typeSet = getTypeSet(context, subTable, paramList->getTypes());
    if (!typeSet) {
      assert(context->hasErrors());
      return std::nullopt;
    }
    const auto typeInstantiation = context->getTypeTemplates()->instantiate(name, *typeSet);
    if (!typeInstantiation) {
      return std::nullopt;
    }
    if (context->hasErrors()) {
      context->reportDiag(errInvalidTypeInstantiation(context->getSrc(), parseType.getSpan()));
      return std::nullopt;
    }
    return (*typeInstantiation)->getType();
  }

  return context->getProg()->lookupType(name);
}

auto getRetType(
    Context* context, const TypeSubstitutionTable* subTable, const parse::FuncDeclStmtNode& n)
    -> std::optional<prog::sym::TypeId> {

  const auto& retTypeSpec = n.getRetType();
  if (!retTypeSpec) {
    return prog::sym::TypeId::inferType();
  }
  auto retType = getOrInstType(context, subTable, retTypeSpec->getType());
  if (!retType) {
    context->reportDiag(errUndeclaredType(
        context->getSrc(), getName(retTypeSpec->getType()), retTypeSpec->getType().getSpan()));
    return std::nullopt;
  }
  return retType;
}

auto inferRetType(
    Context* context,
    const TypeSubstitutionTable* subTable,
    const parse::FuncDeclStmtNode& funcDeclParseNode,
    const prog::sym::TypeSet& input,
    bool aggressive) -> prog::sym::TypeId {

  if (input.getCount() != funcDeclParseNode.getArgs().size()) {
    throw std::invalid_argument{"Incorrect number of input types provided"};
  }

  auto constTypes = std::unordered_map<std::string, prog::sym::TypeId>{};
  for (auto i = 0U; i != input.getCount(); ++i) {
    const auto& argName = getName(funcDeclParseNode.getArgs()[i].getIdentifier());
    constTypes.insert({argName, input[i]});
  }

  auto inferBodyType = TypeInferExpr{context, subTable, &constTypes, aggressive};
  funcDeclParseNode[0].accept(&inferBodyType);
  return inferBodyType.getInferredType();
}

auto getFuncInput(
    Context* context, const TypeSubstitutionTable* subTable, const parse::FuncDeclStmtNode& n)
    -> std::optional<prog::sym::TypeSet> {

  auto isValid  = true;
  auto argTypes = std::vector<prog::sym::TypeId>{};
  for (const auto& arg : n.getArgs()) {
    const auto argType = getOrInstType(context, subTable, arg.getType());
    if (argType) {
      argTypes.push_back(argType.value());
    } else {
      context->reportDiag(
          errUndeclaredType(context->getSrc(), getName(arg.getType()), arg.getType().getSpan()));
      isValid = false;
    }
  }
  return isValid ? std::optional{prog::sym::TypeSet{std::move(argTypes)}} : std::nullopt;
}

auto getSubstitutionParams(Context* context, const parse::TypeSubstitutionList& subList)
    -> std::optional<std::vector<std::string>> {

  auto typeParams = std::vector<std::string>{};
  auto isValid    = true;
  for (const auto& typeSubToken : subList) {
    const auto typeParamName = getName(typeSubToken);
    if (context->getProg()->lookupType(typeParamName)) {
      context->reportDiag(errTypeParamNameConflictsWithType(
          context->getSrc(), typeParamName, typeSubToken.getSpan()));
      isValid = false;
    } else {
      typeParams.push_back(typeParamName);
    }
  }
  return isValid ? std::optional(typeParams) : std::nullopt;
}

auto getTypeSet(
    Context* context,
    const TypeSubstitutionTable* subTable,
    const std::vector<parse::Type>& parseTypes) -> std::optional<prog::sym::TypeSet> {

  auto isValid = true;
  auto types   = std::vector<prog::sym::TypeId>{};
  for (const auto& parseType : parseTypes) {
    auto type = getOrInstType(context, subTable, parseType);
    if (type) {
      types.push_back(*type);
    } else {
      context->reportDiag(
          errUndeclaredType(context->getSrc(), getName(parseType), parseType.getSpan()));
      isValid = false;
    }
  }
  return isValid ? std::optional{prog::sym::TypeSet{std::move(types)}} : std::nullopt;
}

auto getConstName(
    Context* context,
    const TypeSubstitutionTable* subTable,
    const prog::sym::ConstDeclTable& consts,
    const lex::Token& nameToken) -> std::optional<std::string> {

  const auto name = getName(nameToken);
  if (subTable != nullptr && subTable->lookupType(name)) {
    context->reportDiag(
        errConstNameConflictsWithTypeSubstitution(context->getSrc(), name, nameToken.getSpan()));
    return std::nullopt;
  }
  if (context->getProg()->lookupType(name)) {
    context->reportDiag(
        errConstNameConflictsWithType(context->getSrc(), name, nameToken.getSpan()));
    return std::nullopt;
  }
  if (!context->getProg()->lookupFuncs(name).empty()) {
    context->reportDiag(
        errConstNameConflictsWithFunction(context->getSrc(), name, nameToken.getSpan()));
    return std::nullopt;
  }
  if (!context->getProg()->lookupActions(name).empty()) {
    context->reportDiag(
        errConstNameConflictsWithAction(context->getSrc(), name, nameToken.getSpan()));
    return std::nullopt;
  }
  if (consts.lookup(name)) {
    context->reportDiag(
        errConstNameConflictsWithConst(context->getSrc(), name, nameToken.getSpan()));
    return std::nullopt;
  }
  return name;
}

auto mangleName(Context* context, const std::string& name, const prog::sym::TypeSet& typeParams)
    -> std::string {
  auto result = name + '_';
  for (const auto& type : typeParams) {
    const auto& typeName = context->getProg()->getTypeDecl(type).getName();
    result += '_' + typeName;
  }
  return result;
}

auto isConversion(Context* context, const std::string& name) -> bool {
  return context->getProg()->lookupType(name) || context->getTypeTemplates()->hasType(name);
}

} // namespace frontend::internal
