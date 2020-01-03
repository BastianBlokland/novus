#include "utilities.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/context.hpp"
#include "internal/typeinfer_expr.hpp"
#include "parse/type_param_list.hpp"
#include "prog/expr/node_lit_func.hpp"
#include <stdexcept>
#include <unordered_set>

namespace frontend::internal {

auto getName(const lex::Token& token) -> std::string {
  if (token.getKind() != lex::TokenKind::Identifier) {
    return "__unknown";
  }
  return token.getPayload<lex::IdentifierTokenPayload>()->getIdentifier();
}

auto getName(const parse::Type& parseType) -> std::string { return getName(parseType.getId()); }

auto getName(Context* context, prog::sym::TypeId typeId) -> std::string {
  if (!typeId.isConcrete()) {
    return "unknown";
  }
  return context->getProg()->getTypeDecl(typeId).getName();
}

auto getOperator(const lex::Token& token) -> std::optional<prog::Operator> {
  switch (token.getKind()) {
  case lex::TokenKind::OpPlus:
    return prog::Operator::Plus;
  case lex::TokenKind::OpMinus:
    return prog::Operator::Minus;
  case lex::TokenKind::OpStar:
    return prog::Operator::Star;
  case lex::TokenKind::OpSlash:
    return prog::Operator::Slash;
  case lex::TokenKind::OpRem:
    return prog::Operator::Rem;
  case lex::TokenKind::OpAmp:
    return prog::Operator::Amp;
  case lex::TokenKind::OpPipe:
    return prog::Operator::Pipe;
  case lex::TokenKind::OpEqEq:
    return prog::Operator::EqEq;
  case lex::TokenKind::OpBang:
    return prog::Operator::Bang;
  case lex::TokenKind::OpBangEq:
    return prog::Operator::BangEq;
  case lex::TokenKind::OpLe:
    return prog::Operator::Le;
  case lex::TokenKind::OpLeEq:
    return prog::Operator::LeEq;
  case lex::TokenKind::OpGt:
    return prog::Operator::Gt;
  case lex::TokenKind::OpGtEq:
    return prog::Operator::GtEq;
  case lex::TokenKind::OpColonColon:
    return prog::Operator::ColonColon;
  case lex::TokenKind::OpSquareSquare:
    return prog::Operator::SquareSquare;
  case lex::TokenKind::OpParenParen:
    return prog::Operator::ParenParen;
  default:
    return std::nullopt;
  }
}

auto getText(const prog::Operator& op) -> std::string {
  switch (op) {
  case prog::Operator::Plus:
    return "+";
  case prog::Operator::Minus:
    return "-";
  case prog::Operator::Star:
    return "*";
  case prog::Operator::Slash:
    return "/";
  case prog::Operator::Rem:
    return "%";
  case prog::Operator::Amp:
    return "&";
  case prog::Operator::Pipe:
    return "|";
  case prog::Operator::EqEq:
    return "==";
  case prog::Operator::Bang:
    return "!";
  case prog::Operator::BangEq:
    return "!=";
  case prog::Operator::Le:
    return "<";
  case prog::Operator::LeEq:
    return "<=";
  case prog::Operator::Gt:
    return ">";
  case prog::Operator::GtEq:
    return ">=";
  case prog::Operator::ColonColon:
    return "::";
  case prog::Operator::SquareSquare:
    return "[]";
  case prog::Operator::ParenParen:
    return "()";
  }
  return "__unknown";
}

auto isReservedTypeName(const std::string& name) -> bool {
  static const std::unordered_set<std::string> reservedTypes = {
      "int",
      "float",
      "bool",
      "string",
      "func",
  };
  return reservedTypes.find(name) != reservedTypes.end();
}

auto getOrInstType(
    Context* context,
    const TypeSubstitutionTable* subTable,
    const lex::Token& nameToken,
    const std::optional<parse::TypeParamList>& typeParams,
    const prog::sym::TypeSet& constructorArgs) -> std::optional<prog::sym::TypeId> {

  const auto typeName = getName(nameToken);

  // Check if there is an entry in the substitution table for this name.
  if (subTable != nullptr && subTable->lookupType(typeName)) {
    return subTable->lookupType(typeName);
  }
  if (!isType(context, typeName)) {
    return std::nullopt;
  }

  // If type arguments are provided we attempt to instantiate a type using them.
  if (typeParams) {
    return instType(context, subTable, nameToken, *typeParams);
  }

  // Check if the type is non-templated type.
  auto type = context->getProg()->lookupType(typeName);
  if (type) {
    return type;
  }

  // Attempt to instantiate a templated type by infering the type parameters based on the
  // constructor arguments.
  const auto typeInstantiation =
      context->getTypeTemplates()->inferParamsAndInstantiate(typeName, constructorArgs);
  if (typeInstantiation) {
    if (context->hasErrors()) {
      context->reportDiag(errInvalidTypeInstantiation(context->getSrc(), nameToken.getSpan()));
    } else {
      return (*typeInstantiation)->getType();
    }
  }
  return std::nullopt;
}

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
    return instType(context, subTable, parseType.getId(), *paramList);
  }

  return context->getProg()->lookupType(name);
}

auto instType(
    Context* context,
    const TypeSubstitutionTable* subTable,
    const lex::Token& nameToken,
    const parse::TypeParamList& typeParams) -> std::optional<prog::sym::TypeId> {

  const auto typeName = getName(nameToken);
  const auto typeSet  = getTypeSet(context, subTable, typeParams.getTypes());
  if (!typeSet) {
    assert(context->hasErrors());
    return std::nullopt;
  }

  if (typeName == "func") {
    return context->getDelegates()->getDelegate(context, *typeSet);
  }

  const auto typeInstantiation = context->getTypeTemplates()->instantiate(typeName, *typeSet);
  if (!typeInstantiation) {
    return std::nullopt;
  }
  if (context->hasErrors()) {
    context->reportDiag(errInvalidTypeInstantiation(context->getSrc(), nameToken.getSpan()));
    return std::nullopt;
  }
  return (*typeInstantiation)->getType();
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

  if (input.getCount() != funcDeclParseNode.getArgList().getCount()) {
    throw std::invalid_argument{"Incorrect number of input types provided"};
  }

  auto constTypes = std::unordered_map<std::string, prog::sym::TypeId>{};
  for (auto i = 0U; i != input.getCount(); ++i) {
    const auto& argName = getName(funcDeclParseNode.getArgList().getArgs()[i].getIdentifier());
    constTypes.insert({argName, input[i]});
  }

  auto inferBodyType = TypeInferExpr{context, subTable, &constTypes, aggressive};
  funcDeclParseNode[0].accept(&inferBodyType);
  return inferBodyType.getInferredType();
}

auto getLitFunc(Context* context, prog::sym::FuncId func) -> prog::expr::NodePtr {
  const auto funcDecl = context->getProg()->getFuncDecl(func);
  const auto delegateType =
      context->getDelegates()->getDelegate(context, funcDecl.getInput(), funcDecl.getOutput());
  return prog::expr::litFuncNode(*context->getProg(), delegateType, func);
}

auto getFuncInput(
    Context* context, const TypeSubstitutionTable* subTable, const parse::FuncDeclStmtNode& n)
    -> std::optional<prog::sym::TypeSet> {

  auto isValid  = true;
  auto argTypes = std::vector<prog::sym::TypeId>{};
  for (const auto& arg : n.getArgList()) {
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
    if (isType(context, typeParamName)) {
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
  if (context->getProg()->lookupType(name) || isReservedTypeName(name)) {
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

auto isType(Context* context, const std::string& name) -> bool {
  return isReservedTypeName(name) || context->getProg()->lookupType(name) ||
      context->getTypeTemplates()->hasType(name);
}

} // namespace frontend::internal
