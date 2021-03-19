#include "utilities.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/context.hpp"
#include "internal/typeinfer_expr.hpp"
#include "lex/token_payload_id.hpp"
#include "lex/token_payload_keyword.hpp"
#include "lex/token_payload_static_int.hpp"
#include "parse/node_expr_anon_func.hpp"
#include "parse/type_param_list.hpp"
#include "prog/expr/node_closure.hpp"
#include "prog/expr/node_lit_func.hpp"
#include <cassert>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

namespace frontend::internal {

auto getName(const lex::Token& token) -> std::string {
  switch (token.getKind()) {
  case lex::TokenKind::Identifier: {
    return token.getPayload<lex::IdentifierTokenPayload>()->getIdentifier();
  }
  case lex::TokenKind::Keyword: {
    std::ostringstream oss;
    oss << token.getPayload<lex::KeywordTokenPayload>()->getKeyword();
    return oss.str();
  }
  case lex::TokenKind::StaticInt: {
    std::ostringstream oss;
    oss << "#" << token.getPayload<lex::StaticIntTokenPayload>()->getValue();
    return oss.str();
  }
  default:
    return std::string("__unknown");
  }
}

auto getName(const parse::Type& parseType) -> std::string { return getName(parseType.getId()); }

auto getName(const Context& ctx, prog::sym::TypeId typeId) -> std::string {
  if (!typeId.isConcrete()) {
    return "unknown";
  }
  return ctx.getProg()->getTypeDecl(typeId).getName();
}

auto getDisplayName(const Context& ctx, prog::sym::TypeId typeId) -> std::string {
  const auto typeInfo = ctx.getTypeInfo(typeId);
  if (!typeInfo) {
    return getName(ctx, typeId);
  }
  const auto params = typeInfo->getParams();
  if (!params) {
    return typeInfo->getName();
  }
  std::ostringstream oss;
  oss << typeInfo->getName() << '{';
  for (auto i = 0U; i < params->getCount(); ++i) {
    if (i != 0) {
      oss << ',';
    }
    oss << getDisplayName(ctx, (*params)[i]);
  }
  oss << '}';
  return oss.str();
}

auto getOperator(const lex::Token& token) -> std::optional<prog::Operator> {
  switch (token.getKind()) {
  case lex::TokenKind::OpPlus:
    return prog::Operator::Plus;
  case lex::TokenKind::OpPlusPlus:
    return prog::Operator::PlusPlus;
  case lex::TokenKind::OpMinus:
    return prog::Operator::Minus;
  case lex::TokenKind::OpMinusMinus:
    return prog::Operator::MinusMinus;
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
  case lex::TokenKind::OpShiftL:
    return prog::Operator::ShiftL;
  case lex::TokenKind::OpShiftR:
    return prog::Operator::ShiftR;
  case lex::TokenKind::OpHat:
    return prog::Operator::Hat;
  case lex::TokenKind::OpTilde:
    return prog::Operator::Tilde;
  case lex::TokenKind::OpEqEq:
    return prog::Operator::EqEq;
  case lex::TokenKind::OpBang:
    return prog::Operator::Bang;
  case lex::TokenKind::OpBangBang:
    return prog::Operator::BangBang;
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
  case lex::TokenKind::OpQMark:
    return prog::Operator::QMark;
  case lex::TokenKind::OpQMarkQMark:
    return prog::Operator::QMarkQMark;
  default:
    return std::nullopt;
  }
}

auto getText(const prog::Operator& op) -> std::string {
  switch (op) {
  case prog::Operator::Plus:
    return "+";
  case prog::Operator::PlusPlus:
    return "++";
  case prog::Operator::Minus:
    return "-";
  case prog::Operator::MinusMinus:
    return "--";
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
  case prog::Operator::ShiftL:
    return "<<";
  case prog::Operator::ShiftR:
    return ">>";
  case prog::Operator::Hat:
    return "^";
  case prog::Operator::Tilde:
    return "~";
  case prog::Operator::EqEq:
    return "==";
  case prog::Operator::Bang:
    return "!";
  case prog::Operator::BangBang:
    return "!!";
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
  case prog::Operator::QMark:
    return "?";
  case prog::Operator::QMarkQMark:
    return "??";
  }
  return "__unknown";
}

auto isReservedTypeName(const std::string& name) -> bool {
  static const std::unordered_set<std::string> reservedTypes = {
      "int",
      "long",
      "float",
      "bool",
      "string",
      "char",
      "sys_stream",
      "sys_process",
      "function",
      "action",
      "future",
      "lazy",
      "lazy_action",
  };
  return reservedTypes.find(name) != reservedTypes.end();
}

auto getOrInstType(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    const lex::Token& nameToken,
    const std::optional<parse::TypeParamList>& typeParams) -> std::optional<prog::sym::TypeId> {

  const auto typeName = getName(nameToken);

  // Check if there is an entry in the substitution table for this name.
  if (subTable != nullptr && subTable->lookupType(typeName)) {
    return subTable->lookupType(typeName);
  }

  // If type arguments are provided we attempt to instantiate a type using them.
  if (typeParams) {
    return instType(ctx, subTable, nameToken, *typeParams);
  }

  // Check if the type is non-templated type.
  auto type = ctx->getProg()->lookupType(typeName);
  if (type) {
    return type;
  }

  return std::nullopt;
}

auto getOrInstType(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    const lex::Token& nameToken,
    const std::optional<parse::TypeParamList>& typeParams,
    const prog::sym::TypeSet& constructorArgs) -> std::optional<prog::sym::TypeId> {

  // Check if a type can be found / instantiated.
  const auto result = getOrInstType(ctx, subTable, nameToken, typeParams);
  if (result) {
    return result;
  }

  // Attempt to instantiate a templated type by infering the type parameters based on the
  // constructor arguments.
  const auto typeName = getName(nameToken);
  const auto typeInstantiation =
      ctx->getTypeTemplates()->inferParamsAndInstantiate(typeName, constructorArgs);
  if (typeInstantiation) {
    if (!(*typeInstantiation)->isSuccess()) {
      ctx->reportDiag(errInvalidTypeInstantiation, nameToken.getSpan());
    } else {
      return (*typeInstantiation)->getType();
    }
  }
  return std::nullopt;
}

auto getOrInstType(
    Context* ctx, const TypeSubstitutionTable* subTable, const parse::Type& parseType)
    -> std::optional<prog::sym::TypeId> {

  if (parseType.getId().getKind() == lex::TokenKind::StaticInt) {
    return ctx->getStaticIntTable()->getType(
        ctx, parseType.getId().getPayload<lex::StaticIntTokenPayload>()->getValue());
  }

  const auto name = getName(parseType.getId());
  if (subTable) {
    const auto subType = subTable->lookupType(name);
    if (subType) {
      if (parseType.getParamList()) {
        ctx->reportDiag(errTypeParamOnSubstitutionType, parseType.getSpan(), name);
        return std::nullopt;
      }
      return subType;
    }
  }
  auto paramList = parseType.getParamList();
  if (paramList) {
    return instType(ctx, subTable, parseType.getId(), *paramList);
  }

  return ctx->getProg()->lookupType(name);
}

auto instType(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    const lex::Token& nameToken,
    const parse::TypeParamList& typeParams) -> std::optional<prog::sym::TypeId> {

  const auto typeName = getName(nameToken);
  const auto typeSet  = getTypeSet(ctx, subTable, typeParams.getTypes());
  if (!typeSet) {
    assert(ctx->hasErrors());
    return std::nullopt;
  }

  if (typeName == "function") {
    return ctx->getDelegates()->getDelegate(ctx, false, *typeSet);
  }
  if (typeName == "action") {
    return ctx->getDelegates()->getDelegate(ctx, true, *typeSet);
  }
  if (typeName == "future" && typeSet->getCount() == 1) {
    return ctx->getFutures()->getFuture(ctx, *typeSet->begin());
  }
  if (typeName == "lazy" && typeSet->getCount() == 1) {
    return ctx->getLazies()->getLazy(ctx, *typeSet->begin(), false);
  }
  if (typeName == "lazy_action" && typeSet->getCount() == 1) {
    return ctx->getLazies()->getLazy(ctx, *typeSet->begin(), true);
  }

  const auto typeInstantiation = ctx->getTypeTemplates()->instantiate(typeName, *typeSet);
  if (!typeInstantiation) {
    return std::nullopt;
  }
  if (!(*typeInstantiation)->isSuccess()) {
    ctx->reportDiag(errInvalidTypeInstantiation, nameToken.getSpan());
    return std::nullopt;
  }
  return (*typeInstantiation)->getType();
}

auto getRetType(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    const std::optional<parse::RetTypeSpec>& spec) -> std::optional<prog::sym::TypeId> {

  if (!spec) {
    return prog::sym::TypeId::inferType();
  }
  const auto& retParseType = spec->getType();
  auto retType             = getOrInstType(ctx, subTable, retParseType);
  if (!retType) {
    ctx->reportDiag(
        errUndeclaredType,
        retParseType.getSpan(),
        getName(retParseType),
        retParseType.getParamCount());
    return std::nullopt;
  }
  return retType;
}

template <typename FuncParseNode>
auto inferRetType(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    const FuncParseNode& parseNode,
    const prog::sym::TypeSet& input,
    const std::unordered_map<std::string, prog::sym::TypeId>* additionalConstTypes,
    TypeInferExpr::Flags flags) -> prog::sym::TypeId {

  if (input.getCount() != parseNode.getArgList().getCount()) {
    throw std::invalid_argument{"Incorrect number of input types provided"};
  }

  auto constTypes = std::unordered_map<std::string, prog::sym::TypeId>{};
  for (auto i = 0U; i != input.getCount(); ++i) {
    const auto& argList = parseNode.getArgList().getArgs();
    const auto& argName = getName(argList[i].getIdentifier());
    constTypes.insert({argName, input[i]});
  }

  if (additionalConstTypes) {
    constTypes.insert(additionalConstTypes->begin(), additionalConstTypes->end());
  }

  auto inferBodyType = TypeInferExpr{ctx, subTable, &constTypes, flags};
  parseNode.getBody().accept(&inferBodyType);
  return inferBodyType.getInferredType();
}

auto getDelegate(Context* ctx, prog::sym::FuncId func) -> prog::sym::TypeId {
  const auto funcDecl = ctx->getProg()->getFuncDecl(func);
  if (funcDecl.getOutput().isInfer()) {
    return prog::sym::TypeId::inferType();
  }
  return ctx->getDelegates()->getDelegate(
      ctx, funcDecl.isAction(), funcDecl.getInput(), funcDecl.getOutput());
}

auto getLitFunc(Context* ctx, prog::sym::FuncId func) -> prog::expr::NodePtr {
  const auto delType = getDelegate(ctx, func);
  if (delType.isInfer()) {
    return nullptr;
  }
  return prog::expr::litFuncNode(*ctx->getProg(), delType, func);
}

auto getFuncClosure(
    Context* ctx, prog::sym::FuncId func, std::vector<prog::expr::NodePtr> boundArgs)
    -> prog::expr::NodePtr {
  const auto funcDecl = ctx->getProg()->getFuncDecl(func);

  const auto funcInputCnt = funcDecl.getInput().getCount();
  if (boundArgs.size() > funcInputCnt) {
    throw std::logic_error{"More arguments are bound then there are inputs to the function"};
  }
  const auto nonBoundArgsCount = funcDecl.getInput().getCount() - boundArgs.size();

  // Find the input type of the delegate, the bound arguments are not part of the delegate type as
  // they are provided by the closure.
  auto delegateInput = std::vector<prog::sym::TypeId>{};
  for (auto i = 0U; i != funcDecl.getInput().getCount(); ++i) {
    const auto inputType = funcDecl.getInput()[i];
    if (i < nonBoundArgsCount) {
      // Not a bound argument so it becomes part of the delegate input.
      delegateInput.push_back(inputType);
      continue;
    }
    if (inputType != boundArgs[i - nonBoundArgsCount]->getType()) {
      throw std::logic_error{
          "Type of bound argument does not match the type of the function input"};
    }
  }

  auto delegateInputTypes = prog::sym::TypeSet{std::move(delegateInput)};
  const auto delegateType = ctx->getDelegates()->getDelegate(
      ctx, funcDecl.isAction(), delegateInputTypes, funcDecl.getOutput());

  return prog::expr::closureNode(*ctx->getProg(), delegateType, func, std::move(boundArgs));
}

template <typename FuncParseNode>
auto getFuncInput(
    Context* ctx, const TypeSubstitutionTable* subTable, const FuncParseNode& parseNode)
    -> std::optional<prog::sym::TypeSet> {

  auto isValid  = true;
  auto argTypes = std::vector<prog::sym::TypeId>{};
  for (const auto& arg : parseNode.getArgList()) {
    const auto& argParseType = arg.getType();
    const auto argType       = getOrInstType(ctx, subTable, argParseType);
    if (argType) {
      argTypes.push_back(argType.value());
    } else {
      ctx->reportDiag(
          errUndeclaredType,
          argParseType.getSpan(),
          getName(argParseType),
          argParseType.getParamCount());
      isValid = false;
    }
  }
  return isValid ? std::optional{prog::sym::TypeSet{std::move(argTypes)}} : std::nullopt;
}

template <typename FuncParseNode>
auto getNumOptInputs(Context* ctx, const FuncParseNode& parseNode) -> unsigned int {
  auto result = 0u;
  for (const auto& arg : parseNode.getArgList()) {
    if (arg.hasInitializer()) {
      ++result;
      continue;
    }
    if (result) {
      ctx->reportDiag(errNonOptArgFollowingOpt, arg.getSpan());
      return 0u;
    }
  }
  return result;
}

template <typename FuncParseNode>
auto declareFuncInput(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    const FuncParseNode& n,
    prog::sym::ConstDeclTable* consts,
    bool allowArgInitializer) -> bool {
  bool isValid = true;
  for (const auto& arg : n.getArgList()) {
    const auto constName = getConstName(ctx, subTable, *consts, arg.getIdentifier());
    if (!constName) {
      isValid = false;
      continue;
    }
    if (arg.hasInitializer() && !allowArgInitializer) {
      ctx->reportDiag(errUnsupportedArgInitializer, arg.getSpan(), *constName);
      isValid = false;
      continue;
    }

    const auto& argParseType = arg.getType();
    const auto argType       = getOrInstType(ctx, subTable, argParseType);
    if (!argType) {
      ctx->reportDiag(
          errUndeclaredType,
          argParseType.getSpan(),
          getName(argParseType),
          argParseType.getParamCount());
      isValid = false;
      continue;
    }
    consts->registerInput(*constName, argType.value());
  }
  return isValid;
}

auto getSubstitutionParams(Context* ctx, const parse::TypeSubstitutionList& subList)
    -> std::optional<std::vector<std::string>> {

  auto typeParams = std::vector<std::string>{};
  auto isValid    = true;

  // Find all substitution params.
  for (const auto& typeSubToken : subList) {
    const auto typeParamName = getName(typeSubToken);
    if (isType(ctx, nullptr, typeParamName)) {
      ctx->reportDiag(errTypeParamNameConflictsWithType, typeSubToken.getSpan(), typeParamName);
      isValid = false;
    } else {
      typeParams.push_back(typeParamName);
    }
  }

  // Check for duplicates.
  for (auto i = 0u; i < typeParams.size(); ++i) {
    for (auto j = i + 1; j < typeParams.size(); ++j) {
      if (typeParams[i] == typeParams[j]) {
        ctx->reportDiag(errDuplicateTypeParamName, subList.getSpan(), typeParams[i]);
        isValid = false;
      }
    }
  }

  return isValid ? std::optional(typeParams) : std::nullopt;
}

auto getTypeSet(
    Context* ctx, const TypeSubstitutionTable* subTable, const std::vector<parse::Type>& parseTypes)
    -> std::optional<prog::sym::TypeSet> {

  auto isValid = true;
  auto types   = std::vector<prog::sym::TypeId>{};
  for (const auto& parseType : parseTypes) {
    auto type = getOrInstType(ctx, subTable, parseType);
    if (type) {
      types.push_back(*type);
    } else {
      ctx->reportDiag(
          errUndeclaredType, parseType.getSpan(), getName(parseType), parseType.getParamCount());
      isValid = false;
    }
  }
  return isValid ? std::optional{prog::sym::TypeSet{std::move(types)}} : std::nullopt;
}

auto getTypeSet(const std::vector<prog::expr::NodePtr>& exprs) -> prog::sym::TypeSet {
  auto types = std::vector<prog::sym::TypeId>{};
  for (const auto& expr : exprs) {
    types.push_back(expr->getType());
  }
  return prog::sym::TypeSet{std::move(types)};
}

auto getConstName(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    const prog::sym::ConstDeclTable& consts,
    const lex::Token& nameToken) -> std::optional<std::string> {

  const auto name = getName(nameToken);
  if (subTable != nullptr && subTable->lookupType(name)) {
    ctx->reportDiag(errConstNameConflictsWithTypeSubstitution, nameToken.getSpan(), name);
    return std::nullopt;
  }
  if (isType(ctx, nullptr, name)) {
    ctx->reportDiag(errConstNameConflictsWithType, nameToken.getSpan(), name);
    return std::nullopt;
  }
  if (consts.lookup(name)) {
    ctx->reportDiag(errConstNameConflictsWithConst, nameToken.getSpan(), name);
    return std::nullopt;
  }
  return name;
}

auto mangleName(Context* ctx, const std::string& name, const prog::sym::TypeSet& typeParams)
    -> std::string {
  auto result = name + '_';
  for (const auto& type : typeParams) {
    const auto& typeName =
        type.isConcrete() ? ctx->getProg()->getTypeDecl(type).getName() : "error";
    result += '_' + typeName;
  }
  return result;
}

auto asFuture(Context* ctx, prog::sym::TypeId type) -> prog::sym::TypeId {
  return ctx->getFutures()->getFuture(ctx, type);
}

auto funcOutAsFuture(Context* ctx, prog::sym::FuncId func) -> prog::sym::TypeId {
  return asFuture(ctx, ctx->getProg()->getFuncDecl(func).getOutput());
}

auto delegateOutAsFuture(Context* ctx, prog::sym::TypeId delegate)
    -> std::optional<prog::sym::TypeId> {
  const auto delOut = ctx->getProg()->getDelegateRetType(delegate);
  if (delOut) {
    return asFuture(ctx, *delOut);
  }
  return std::nullopt;
}

auto asLazy(Context* ctx, prog::sym::TypeId type, bool isAction) -> prog::sym::TypeId {
  return ctx->getLazies()->getLazy(ctx, type, isAction);
}

auto funcOutAsLazy(Context* ctx, prog::sym::FuncId func) -> prog::sym::TypeId {
  const auto& funcDecl = ctx->getProg()->getFuncDecl(func);
  return asLazy(ctx, funcDecl.getOutput(), funcDecl.isAction());
}

auto delegateOutAsLazy(Context* ctx, prog::sym::TypeId delegate)
    -> std::optional<prog::sym::TypeId> {
  const auto delOut = ctx->getProg()->getDelegateRetType(delegate);
  if (delOut) {
    return asLazy(ctx, *delOut, ctx->getProg()->isActionDelegate(delegate));
  }
  return std::nullopt;
}

auto isType(Context* ctx, const TypeSubstitutionTable* subTable, const std::string& name) -> bool {
  return isReservedTypeName(name) || ctx->getProg()->hasType(name) ||
      ctx->getTypeTemplates()->hasType(name) || (subTable && subTable->lookupType(name));
}

auto isFuncOrConv(Context* ctx, const TypeSubstitutionTable* subTable, const std::string& name)
    -> bool {
  return isType(ctx, subTable, name) || ctx->getProg()->hasFunc(name) ||
      ctx->getFuncTemplates()->hasFunc(name);
}

auto isPure(const Context* ctx, prog::sym::FuncId func) -> bool { return !isAction(ctx, func); }

auto isAction(const Context* ctx, prog::sym::FuncId func) -> bool {
  return ctx->getProg()->getFuncDecl(func).isAction();
}

// Explicit instantiations.
template prog::sym::TypeId inferRetType(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    const parse::FuncDeclStmtNode& parseNode,
    const prog::sym::TypeSet& input,
    const std::unordered_map<std::string, prog::sym::TypeId>* additionalConstTypes,
    TypeInferExpr::Flags flags);
template prog::sym::TypeId inferRetType(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    const parse::AnonFuncExprNode& parseNode,
    const prog::sym::TypeSet& input,
    const std::unordered_map<std::string, prog::sym::TypeId>* additionalConstTypes,
    TypeInferExpr::Flags flags);

template std::optional<prog::sym::TypeSet>
getFuncInput(Context* ctx, const TypeSubstitutionTable* subTable, const parse::FuncDeclStmtNode& n);
template std::optional<prog::sym::TypeSet>
getFuncInput(Context* ctx, const TypeSubstitutionTable* subTable, const parse::AnonFuncExprNode& n);

template unsigned int getNumOptInputs(Context* ctx, const parse::FuncDeclStmtNode& n);
template unsigned int getNumOptInputs(Context* ctx, const parse::AnonFuncExprNode& n);

template bool declareFuncInput(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    const parse::FuncDeclStmtNode& n,
    prog::sym::ConstDeclTable* consts,
    bool allowArgInitializer);
template bool declareFuncInput(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    const parse::AnonFuncExprNode& n,
    prog::sym::ConstDeclTable* consts,
    bool allowArgInitializer);

} // namespace frontend::internal
