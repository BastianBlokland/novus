#include "internal/get_expr.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/check_union_exhaustiveness.hpp"
#include "internal/define_user_funcs.hpp"
#include "internal/get_identifier.hpp"
#include "internal/intrinsic_call_hooks.hpp"
#include "internal/utilities.hpp"
#include "lex/token_payload_lit_bool.hpp"
#include "lex/token_payload_lit_char.hpp"
#include "lex/token_payload_lit_float.hpp"
#include "lex/token_payload_lit_int.hpp"
#include "lex/token_payload_lit_long.hpp"
#include "lex/token_payload_lit_string.hpp"
#include "parse/nodes.hpp"
#include "prog/expr/nodes.hpp"
#include <cassert>
#include <sstream>

namespace frontend::internal {

GetExpr::GetExpr(
    Context* ctx,
    const TypeSubstitutionTable* typeSubTable,
    ConstBinder* constBinder,
    prog::sym::TypeId typeHint,
    std::optional<Signature> selfSig,
    Flags flags) :
    m_ctx{ctx},
    m_typeSubTable{typeSubTable},
    m_constBinder{constBinder},
    m_typeHint{typeHint},
    m_selfSig{std::move(selfSig)},
    m_flags{flags} {

  if (m_ctx == nullptr) {
    throw std::invalid_argument{"Context cannot be null"};
  }
}

auto GetExpr::getValue() -> prog::expr::NodePtr& { return m_expr; }

auto GetExpr::visit(const parse::CommentNode& /*unused*/) -> void {
  throw std::logic_error{"GetExpr is not implemented for this node type"};
}

auto GetExpr::visit(const parse::ErrorNode& /*unused*/) -> void {
  throw std::logic_error{"GetExpr is not implemented for this node type"};
}

auto GetExpr::visit(const parse::AnonFuncExprNode& n) -> void {
  auto consts = prog::sym::ConstDeclTable{};

  // Declare the input types in the cost table.
  if (!declareFuncInput(m_ctx, m_typeSubTable, n, &consts, false)) {
    assert(m_ctx->hasErrors());
    return;
  }

  // Gather the input types for the anonymous function.
  auto inputTypes = std::vector<prog::sym::TypeId>{};
  for (const auto& constId : consts.getNonBoundInputs()) {
    inputTypes.push_back(consts[constId].getType());
  }

  // Impure lambda's produce actions and are allowed to call other actions.
  auto isAction = n.isImpure();

  // Find the return-type, either from a explicit specification or inferred.
  auto retType = getRetType(m_ctx, m_typeSubTable, n.getRetType());
  if (!retType) {
    assert(m_ctx->hasErrors());
    return;
  }
  if (retType->isInfer()) {
    // Infer the return type of the anonymous function.
    auto parentConstTypes = m_constBinder ? m_constBinder->getAllConstTypes()
                                          : std::unordered_map<std::string, prog::sym::TypeId>{};

    retType = inferRetType(
        m_ctx,
        m_typeSubTable,
        n,
        prog::sym::TypeSet{inputTypes},
        &parentConstTypes,
        isAction ? (TypeInferExpr::Flags::AllowActionCalls | TypeInferExpr::Flags::Aggressive)
                 : TypeInferExpr::Flags::Aggressive);

    // Note: We do not fail yet if we fail to infer the type, reason is it would 'hide' many other
    // errors that would otherwise be reported by analyzing the body.
  }

  // Construct the signature, used to enable self-calls.
  auto selfSignature = std::make_pair(prog::sym::TypeSet{inputTypes}, *retType);

  // Analyze the body of the anonymous function.
  auto visibleConsts = consts.getAll();
  auto constBinder   = ConstBinder{&consts, &visibleConsts, m_constBinder};
  auto getExpr       = GetExpr{
      m_ctx,
      m_typeSubTable,
      &constBinder,
      *retType,
      selfSignature,
      isAction ? (Flags::AllowActionCalls | Flags::AllowPureFuncCalls) : Flags::AllowPureFuncCalls};
  n.getBody().accept(&getExpr);
  if (!getExpr.m_expr) {
    assert(m_ctx->hasErrors());
    return;
  }
  auto expr = std::move(getExpr.m_expr);

  // Fail if return type inference failed.
  if (!retType->isConcrete()) {
    m_ctx->reportDiag(errUnableToInferLambdaReturnType, n.getSpan());
    return;
  }

  // Apply an implicit conversion if neccesary.
  if (!applyImplicitConversion(&expr, *retType, n.getSpan())) {
    assert(m_ctx->hasErrors());
    return;
  }

  // Add the bound input types for this function.
  for (const auto& constId : consts.getBoundInputs()) {
    inputTypes.push_back(consts[constId].getType());
  }

  // Generate a unique function name.
  std::ostringstream nameoss;
  nameoss << "__anon_" << m_ctx->getProg()->getFuncCount();

  // Declare the function in the program.
  const auto funcId = isAction
      ? m_ctx->getProg()->declareAction(nameoss.str(), prog::sym::TypeSet{inputTypes}, *retType, 0u)
      : m_ctx->getProg()->declarePureFunc(
            nameoss.str(), prog::sym::TypeSet{inputTypes}, *retType, 0u);

  const auto isNoInline = n.hasModifier(lex::Keyword::Noinline);
  const auto funcDefFlags =
      isNoInline ? prog::sym::FuncDef::Flags::NoInline : prog::sym::FuncDef::Flags::None;

  // Define the function in the program.
  m_ctx->getProg()->defineFunc(funcId, std::move(consts), std::move(expr), {}, funcDefFlags);

  // Either create a function literal or a closure, depending on if the anonymous func binds any
  // consts from the parent.
  if (constBinder.getBoundParentConsts().empty()) {
    m_expr = getLitFunc(m_ctx, funcId);
    m_ctx->associateSrc(m_expr, n.getSpan());
  } else {
    auto boundArgs = std::vector<prog::expr::NodePtr>{};
    if (m_constBinder) {
      // Collect any bound arguments.
      for (const auto& parentConstId : constBinder.getBoundParentConsts()) {
        boundArgs.push_back(prog::expr::constExprNode(*m_constBinder->getConsts(), parentConstId));
      }
    }
    m_expr = getFuncClosure(m_ctx, funcId, std::move(boundArgs));
    m_ctx->associateSrc(m_expr, n.getSpan());
  }
}

auto GetExpr::visit(const parse::BinaryExprNode& n) -> void {
  const auto& opToken = n.getOperator();
  switch (opToken.getKind()) {
  case lex::TokenKind::OpAmpAmp:
    m_expr = getBinLogicOpExpr(n, BinLogicOp::And);
    m_ctx->associateSrc(m_expr, n.getSpan());
    return;
  case lex::TokenKind::OpPipePipe:
    m_expr = getBinLogicOpExpr(n, BinLogicOp::Or);
    m_ctx->associateSrc(m_expr, n.getSpan());
    return;
  default:
    break;
  }
  const auto op = getOperator(opToken);
  if (!op) {
    m_ctx->reportDiag(errUnsupportedOperator, opToken.getSpan(), opToken.str());
    return;
  }

  auto args = std::vector<prog::expr::NodePtr>{};
  args.push_back(getSubExpr(n[0], prog::sym::TypeId::inferType()));
  args.push_back(getSubExpr(n[1], prog::sym::TypeId::inferType()));
  if (!args[0] || !args[1] || !args[0]->getType().isConcrete() ||
      !args[1]->getType().isConcrete()) {
    return;
  }

  const auto argTypeSet = prog::sym::TypeSet{{args[0]->getType(), args[1]->getType()}};
  const auto ovOptions  = getOvOptions(1);
  const auto funcName   = prog::getFuncName(op.value());
  const auto possibleFuncs =
      getFunctions(funcName, std::nullopt, argTypeSet, n.getOperator().getSpan(), ovOptions);
  const auto func = m_ctx->getProg()->lookupFunc(possibleFuncs, argTypeSet, ovOptions);
  if (!func) {
    m_ctx->reportDiag(
        errUndeclaredBinOperator,
        opToken.getSpan(),
        getText(op.value()),
        getDisplayName(*m_ctx, args[0]->getType()),
        getDisplayName(*m_ctx, args[1]->getType()));
    return;
  }

  m_expr = prog::expr::callExprNode(*m_ctx->getProg(), func.value(), std::move(args));
  m_ctx->associateSrc(m_expr, n.getSpan());
}

auto GetExpr::visit(const parse::CallExprNode& n) -> void {
  auto getIdVisitor = GetIdentifier{true};
  n[0].accept(&getIdVisitor);
  auto* instance  = getIdVisitor.getInstance();
  auto identifier = getIdVisitor.getIdentifier();
  auto typeParams = getIdVisitor.getTypeParams();

  /* Check what kind of call this is.
  - If lhs is 'self': Self call.
  - If lhs is an intrinsic: Static call to intrinsic.
  - If lhs is not an identifier (but some other expression): Dynamic call.
  - Lhs is an identifier that matches a constant name: Dynamic call.
  - Lhs has an instance (like a struct) and its (field) name is not a function: Dynamic call.
  - Otherwise: Static call (If there is an lhs instance its treated as the first arg). */

  if (getIdVisitor.isSelf()) {
    m_expr = getSelfCallExpr(n);
    m_ctx->associateSrc(m_expr, n.getSpan());
    return;
  }

  const auto isBindableConstant = m_constBinder && identifier && !getIdVisitor.isIntrinsic() &&
      m_constBinder->canBind(getName(*identifier));
  // NOTE: Is used to choose between a 'instance' call or a call to a delegate field on a struct.
  const auto isFuncOrConvName = identifier && !getIdVisitor.isIntrinsic() &&
      isFuncOrConv(m_ctx, m_typeSubTable, getName(*identifier));

  if (!identifier || (instance == nullptr && isBindableConstant) ||
      (instance != nullptr && !isFuncOrConvName)) {
    m_expr = getDynCallExpr(n);
    m_ctx->associateSrc(m_expr, n.getSpan());
    return;
  }

  assert(identifier);

  auto nameToken  = *identifier;
  auto nameString = getName(nameToken);
  auto args       = getChildExprs(n, instance, 1);
  if (!args) {
    return;
  }

  const bool allowPureFunc = hasFlag<Flags::AllowPureFuncCalls>();
  const bool allowAction   = hasFlag<Flags::AllowActionCalls>();

  if (getIdVisitor.isIntrinsic()) {
    // Meta intrinsics are intrinsics where the frontend emits special expression nodes, an example
    // of this is 'intrinsic{type_name}{T}()' where the frontend will actually emit a literal string
    // expression containing the type name.
    if (auto metaExpr =
            resolveMetaIntrinsic(m_ctx, m_typeSubTable, nameToken, typeParams, *args.get())) {

      m_expr = std::move(*metaExpr);
      m_ctx->associateSrc(m_expr, n.getSpan());
      return;
    }
  }

  // On instance calls we do not allow (implicit) conversions on the first argument (the instance).
  const auto noConvFirstArg = instance != nullptr;
  const auto ovOptions      = getOvOptions(-1, false, noConvFirstArg);

  auto possibleFuncs = getIdVisitor.isIntrinsic()
      ? m_ctx->getProg()->lookupIntrinsic(getName(nameToken), ovOptions)
      : getFunctionsInclConversions(nameToken, typeParams, args->second, ovOptions);

  const auto func = m_ctx->getProg()->lookupFunc(possibleFuncs, args->second, ovOptions);
  if (!func) {
    auto argTypeNames = getDisplayNames(*m_ctx, args->second);
    if (auto type = getOrInstType(m_ctx, m_typeSubTable, nameToken, typeParams)) {
      m_ctx->reportDiag(
          errUndeclaredTypeOrConversion, n.getSpan(), getDisplayName(*m_ctx, *type), argTypeNames);
      return;
    }
    if (isType(m_ctx, m_typeSubTable, nameString)) {
      // Tried to call a constructor but we do not know which type (probably because the user
      // provided incorrect type-params or we could not infer the type-params).
      if (typeParams) {
        // TODO: Include the type-names for the type-parameters in the diagnostic.
        // TODO: Include the type-names for the arguments in the diagnostic.
        m_ctx->reportDiag(
            errNoTypeOrConversionFoundToInstantiate,
            n.getSpan(),
            nameString,
            typeParams->getCount());
      } else {
        m_ctx->reportDiag(errUndeclaredTypeOrConversion, n.getSpan(), nameString, argTypeNames);
      }
      return;
    }

    if (typeParams) {
      const auto typeParamSet = getTypeSet(m_ctx, m_typeSubTable, typeParams->getTypes());
      auto typeParamNames =
          typeParamSet ? getDisplayNames(*m_ctx, *typeParamSet) : std::vector<std::string>{};

      // TODO: Include the type-names for the arguments in the diagnostic.
      if (getIdVisitor.isIntrinsic()) {
        m_ctx->reportDiag(errNoCompatibleIntrinsicFound, n.getSpan(), nameString, typeParamNames);
      } else if (allowPureFunc && allowAction) {
        m_ctx->reportDiag(
            errNoFuncOrActionFoundToInstantiate, n.getSpan(), nameString, typeParamNames);
      } else if (allowPureFunc) {
        m_ctx->reportDiag(errNoPureFuncFoundToInstantiate, n.getSpan(), nameString, typeParamNames);
      } else {
        m_ctx->reportDiag(errNoActionFoundToInstantiate, n.getSpan(), nameString, typeParamNames);
      }
      return;
    }
    if (getIdVisitor.isIntrinsic()) {
      m_ctx->reportDiag(errUnknownIntrinsic, n.getSpan(), nameString, !allowAction, argTypeNames);
    } else if (allowPureFunc && allowAction) {
      m_ctx->reportDiag(errUndeclaredFuncOrAction, n.getSpan(), nameString, argTypeNames);
    } else if (allowPureFunc) {
      m_ctx->reportDiag(errUndeclaredPureFunc, n.getSpan(), nameString, argTypeNames);
    } else {
      m_ctx->reportDiag(errUndeclaredAction, n.getSpan(), nameString, argTypeNames);
    }
    return;
  }

  if (n.isFork() || n.isLazy()) {
    assert(n.isFork() != n.isLazy());

    if (m_ctx->getProg()->getFuncDecl(*func).getKind() != prog::sym::FuncKind::User) {
      m_ctx->reportDiag(n.isFork() ? errForkedNonUserFunc : errLazyNonUserFunc, n.getSpan());
      return;
    }

    m_expr = prog::expr::callExprNode(
        *m_ctx->getProg(),
        func.value(),
        n.isFork() ? funcOutAsFuture(m_ctx, *func) : funcOutAsLazy(m_ctx, *func),
        std::move(args->first),
        n.isFork() ? prog::expr::CallMode::Forked : prog::expr::CallMode::Lazy);
  } else {
    m_expr = prog::expr::callExprNode(*m_ctx->getProg(), *func, std::move(args->first));
  }
  m_ctx->associateSrc(m_expr, n.getSpan());
}

auto GetExpr::visit(const parse::ConditionalExprNode& n) -> void {
  auto conditions = std::vector<prog::expr::NodePtr>{};
  auto branches   = std::vector<prog::expr::NodePtr>{};

  auto conditionConsts = getVisibleConstsCopy();
  conditions.push_back(getSubExpr(n[0], &conditionConsts, m_ctx->getProg()->getBool(), true));
  if (!conditions[0]) {
    return;
  }
  if (!applyImplicitConversion(&conditions[0], m_ctx->getProg()->getBool(), n[0].getSpan())) {
    return;
  }

  // The 'if' branch gets access to the constants declared by the condition.
  branches.push_back(getSubExpr(n[1], &conditionConsts, m_typeHint));

  // The 'else' branch only get constants that are visible in the parent scope.
  auto elseBranchConsts = getVisibleConstsCopy();
  branches.push_back(getSubExpr(n[2], &elseBranchConsts, m_typeHint));

  if (!branches[0] || !branches[1]) {
    return;
  }

  // If no preference is given over the desired output type then check what type the branches
  // have in common.
  if (m_typeHint.isInfer()) {
    const auto branchTypes = std::vector{branches[0]->getType(), branches[1]->getType()};
    auto commonType        = m_ctx->getProg()->findCommonType(branchTypes);
    if (!commonType) {
      m_ctx->reportDiag(errBranchesHaveNoCommonType, n.getSpan());
      return;
    }
    m_typeHint = *commonType;
  }

  // Add a conversion for all branches that require one.
  for (auto i = 0U; i != branches.size(); ++i) {
    if (!applyImplicitConversion(&branches[i], m_typeHint, n[i].getSpan())) {
      return;
    }
  }

  m_expr =
      prog::expr::switchExprNode(*m_ctx->getProg(), std::move(conditions), std::move(branches));
  m_ctx->associateSrc(m_expr, n.getSpan());
}

auto GetExpr::visit(const parse::ConstDeclExprNode& n) -> void {
  if (!m_constBinder) {
    m_ctx->reportDiag(errConstDeclareNotSupported, n.getSpan());
    return;
  }

  auto assignExpr = getSubExpr(n[0], prog::sym::TypeId::inferType());
  if (assignExpr == nullptr) {
    return;
  }
  const auto constId = declareConst(n.getId(), assignExpr->getType());
  if (constId) {
    m_expr = prog::expr::assignExprNode(
        *m_constBinder->getConsts(), constId.value(), std::move(assignExpr));
    m_ctx->associateSrc(m_expr, n.getSpan());
  }
}

auto GetExpr::visit(const parse::IdExprNode& n) -> void {
  const auto name = getName(n.getId());
  // Check if this is a constant, if so bind to it.
  if (m_constBinder && m_constBinder->canBind(name)) {
    m_expr = getConstExpr(n);
    m_ctx->associateSrc(m_expr, n.getSpan());
    assert(m_expr != nullptr || m_ctx->hasErrors());
    return;
  }

  // Templated function literal.
  if (n.getTypeParams()) {
    const auto typeSet = getTypeSet(m_ctx, m_typeSubTable, n.getTypeParams()->getTypes());
    if (!typeSet) {
      assert(m_ctx->hasErrors());
      return;
    }
    const auto instances = m_ctx->getFuncTemplates()->instantiate(
        name, *typeSet, prog::OvOptions{prog::OvFlags::ExclNonUser, -1});

    if (instances.empty()) {
      const auto typeParamNames = getDisplayNames(*m_ctx, *typeSet);
      m_ctx->reportDiag(errNoFuncOrActionFoundToInstantiate, n.getSpan(), name, typeParamNames);
      return;
    }
    if (instances.size() != 1) {
      m_ctx->reportDiag(errAmbiguousTemplateFunction, n.getSpan(), name, typeSet->getCount());
      return;
    }
    m_expr = getLitFunc(m_ctx, *instances[0]->getFunc());
    m_ctx->associateSrc(m_expr, n.getSpan());
    return;
  }

  // Non-templated function literal.
  auto funcs = m_ctx->getProg()->lookupFunc(name, prog::OvOptions{prog::OvFlags::ExclNonUser, -1});
  if (!funcs.empty()) {
    if (funcs.size() != 1) {
      m_ctx->reportDiag(errAmbiguousFunction, n.getSpan(), name);
      return;
    }
    m_expr = getLitFunc(m_ctx, funcs[0]);
    m_ctx->associateSrc(m_expr, n.getSpan());
    return;
  }

  // Error if no templated args are provided to templated function.
  if (m_ctx->getFuncTemplates()->hasFunc(name)) {
    m_ctx->reportDiag(errNoTypeParamsProvidedToTemplateFunction, n.getSpan(), name);
    return;
  }

  // If its not a function literal treat it as a constant, even tho we already checked if this is
  // a constant at the top of this function we would rather have the error be 'no constant found'
  // then no function found.
  m_expr = getConstExpr(n);
  m_ctx->associateSrc(m_expr, n.getSpan());
  assert(m_expr != nullptr || m_ctx->hasErrors());
}

auto GetExpr::visit(const parse::FieldExprNode& n) -> void {
  // Check if the lhs is a type name token, in that case treat this as access to a 'static' field.
  auto getIdVisitor = GetIdentifier{false};
  n[0].accept(&getIdVisitor);
  auto identifier = getIdVisitor.getIdentifier();
  if (identifier && !getIdVisitor.isIntrinsic() &&
      isType(m_ctx, m_typeSubTable, getName(*identifier))) {
    m_expr = getStaticFieldExpr(*identifier, getIdVisitor.getTypeParams(), n.getId());
    m_ctx->associateSrc(m_expr, n.getSpan());
    return;
  }

  // If not a static field then acces the field on the lhs expression.
  auto lhsExpr = getSubExpr(n[0], prog::sym::TypeId::inferType());
  if (lhsExpr == nullptr) {
    return;
  }

  const auto fieldName = getName(n.getId());
  const auto lhsType   = lhsExpr->getType();
  if (!lhsType.isConcrete()) {
    m_ctx->reportDiag(
        errFieldNotFoundOnType, n.getSpan(), fieldName, getDisplayName(*m_ctx, lhsType));
    return;
  }

  const auto& lhsTypeDecl = m_ctx->getProg()->getTypeDecl(lhsType);
  if (lhsTypeDecl.getKind() != prog::sym::TypeKind::Struct) {
    m_ctx->reportDiag(
        errFieldNotFoundOnType, n.getSpan(), fieldName, getDisplayName(*m_ctx, lhsType));
    return;
  }
  const auto& structDef = std::get<prog::sym::StructDef>(m_ctx->getProg()->getTypeDef(lhsType));
  const auto field      = structDef.getFields().lookup(fieldName);
  if (!field) {
    m_ctx->reportDiag(
        errFieldNotFoundOnType, n.getSpan(), fieldName, getDisplayName(*m_ctx, lhsType));
    return;
  }

  m_expr = prog::expr::fieldExprNode(*m_ctx->getProg(), std::move(lhsExpr), *field);
  m_ctx->associateSrc(m_expr, n.getSpan());
}

auto GetExpr::visit(const parse::GroupExprNode& n) -> void {
  auto subExprs = std::vector<prog::expr::NodePtr>();
  for (auto i = 0U; i < n.getChildCount(); ++i) {
    auto last    = i == n.getChildCount() - 1;
    auto subExpr = getSubExpr(
        n[i],
        last ? m_typeHint : prog::sym::TypeId::inferType(),
        hasFlag<Flags::CheckedConstsAccess>() && last);
    if (subExpr) {
      subExprs.push_back(std::move(subExpr));
    }
  }
  if (subExprs.size() > 1) {
    m_expr = prog::expr::groupExprNode(std::move(subExprs));
    m_ctx->associateSrc(m_expr, n.getSpan());
  }
}

auto GetExpr::visit(const parse::IndexExprNode& n) -> void {
  auto args = getChildExprs(n);
  if (!args) {
    assert(m_ctx->hasErrors());
    return;
  }

  const auto funcName  = prog::getFuncName(prog::Operator::SquareSquare);
  const auto ovOptions = getOvOptions(-1);
  const auto possibleFuncs =
      getFunctions(funcName, std::nullopt, args->second, n.getSpan(), ovOptions);
  const auto func = m_ctx->getProg()->lookupFunc(possibleFuncs, args->second, ovOptions);
  if (!func) {
    auto argTypeNames = std::vector<std::string>{};
    for (const auto& argType : args->second) {
      argTypeNames.push_back(getDisplayName(*m_ctx, argType));
    }
    m_ctx->reportDiag(errUndeclaredIndexOperator, n.getSpan(), argTypeNames);
    return;
  }

  m_expr = prog::expr::callExprNode(*m_ctx->getProg(), func.value(), std::move(args->first));
  m_ctx->associateSrc(m_expr, n.getSpan());
}

auto GetExpr::visit(const parse::IntrinsicExprNode& n) -> void {
  // Compiler intrinsics are only callable, they are not valid as function literals.
  m_ctx->reportDiag(errIntrinsicFuncLiteral, n.getSpan());
}

auto GetExpr::visit(const parse::IsExprNode& n) -> void {
  auto lhsExpr = getSubExpr(n[0], prog::sym::TypeId::inferType());
  if (lhsExpr == nullptr) {
    return;
  }

  // Validate lhs is a union type.
  const auto lhsType = lhsExpr->getType();
  if (lhsType.isInfer()) {
    return;
  }

  const auto& lhsTypeDecl = m_ctx->getProg()->getTypeDecl(lhsType);
  if (lhsTypeDecl.getKind() != prog::sym::TypeKind::Union) {
    m_ctx->reportDiag(errNonUnionIsExpression, n[0].getSpan());
    return;
  }
  const auto& unionDef = std::get<prog::sym::UnionDef>(m_ctx->getProg()->getTypeDef(lhsType));

  // Validate that the type is a valid type.
  const auto& parseType = n.getType();
  const auto type       = getOrInstType(m_ctx, m_typeSubTable, parseType);
  if (!type) {
    m_ctx->reportDiag(
        errUndeclaredType, parseType.getSpan(), getName(parseType), parseType.getParamCount());
    return;
  }

  // Validate that the type is part of the union.
  if (!unionDef.hasType(*type)) {
    m_ctx->reportDiag(
        errTypeNotPartOfUnion,
        parseType.getSpan(),
        getDisplayName(*m_ctx, *type),
        getDisplayName(*m_ctx, lhsType));
    return;
  }

  if (!n.hasId()) {
    m_expr = prog::expr::unionCheckExprNode(*m_ctx->getProg(), std::move(lhsExpr), *type);
    m_ctx->associateSrc(m_expr, n.getSpan());
    return;
  }

  if (!m_constBinder) {
    m_ctx->reportDiag(errConstDeclareNotSupported, n.getSpan());
    return;
  }

  // Validate that this expression is part of a checked context, meaning the const is only
  // accessed when the expression evaluates to 'true'.
  if (!hasFlag<Flags::CheckedConstsAccess>()) {
    m_ctx->reportDiag(errUncheckedAsExpressionWithConst, parseType.getSpan());
    return;
  }

  // Declare the const.
  const auto constId = declareConst(*n.getId(), *type);
  if (constId) {
    m_expr = prog::expr::unionGetExprNode(
        *m_ctx->getProg(), std::move(lhsExpr), *m_constBinder->getConsts(), *constId);
    m_ctx->associateSrc(m_expr, n.getSpan());
  }
}

auto GetExpr::visit(const parse::LitExprNode& n) -> void {
  switch (n.getVal().getKind()) {
  case lex::TokenKind::LitInt:
    m_expr = prog::expr::litIntNode(
        *m_ctx->getProg(), n.getVal().getPayload<lex::LitIntTokenPayload>()->getValue());
    break;
  case lex::TokenKind::LitLong:
    m_expr = prog::expr::litLongNode(
        *m_ctx->getProg(), n.getVal().getPayload<lex::LitLongTokenPayload>()->getValue());
    break;
  case lex::TokenKind::LitFloat:
    m_expr = prog::expr::litFloatNode(
        *m_ctx->getProg(), n.getVal().getPayload<lex::LitFloatTokenPayload>()->getValue());
    break;
  case lex::TokenKind::LitBool:
    m_expr = prog::expr::litBoolNode(
        *m_ctx->getProg(), n.getVal().getPayload<lex::LitBoolTokenPayload>()->getValue());
    break;
  case lex::TokenKind::LitString:
    m_expr = prog::expr::litStringNode(
        *m_ctx->getProg(), n.getVal().getPayload<lex::LitStringTokenPayload>()->getValue());
    break;
  case lex::TokenKind::LitChar:
    m_expr = prog::expr::litCharNode(
        *m_ctx->getProg(), n.getVal().getPayload<lex::LitCharTokenPayload>()->getValue());
    break;
  default:
    std::stringstream oss;
    oss << n.getVal().getKind();
    m_ctx->reportDiag(errUnsupportedLiteral, n.getSpan(), oss.str());
    return;
  }
  m_ctx->associateSrc(m_expr, n.getSpan());
}

auto GetExpr::visit(const parse::ParenExprNode& n) -> void {
  m_expr = getSubExpr(n[0], m_typeHint, hasFlag<Flags::CheckedConstsAccess>());
  m_ctx->associateSrc(m_expr, n.getSpan());
}

auto GetExpr::visit(const parse::SwitchExprElseNode& /*unused*/) -> void {
  throw std::logic_error{"GetExpr is not implemented for this node type"};
}

auto GetExpr::visit(const parse::SwitchExprIfNode& /*unused*/) -> void {
  throw std::logic_error{"GetExpr is not implemented for this node type"};
}

auto GetExpr::visit(const parse::SwitchExprNode& n) -> void {
  auto isValid     = true;
  auto conditions  = std::vector<prog::expr::NodePtr>();
  auto branches    = std::vector<prog::expr::NodePtr>();
  auto branchTypes = std::vector<prog::sym::TypeId>{};

  for (auto i = 0U; i < n.getChildCount(); ++i) {
    const auto isElseClause = n.hasElse() && i == n.getChildCount() - 1;

    // Keep a separate set of visible constants, because consts declared in 1 branch should not
    // be allowed to be accessed from another branch or after the switch.
    auto branchConsts = getVisibleConstsCopy();
    if (!isElseClause) {
      auto condition = getSubExpr(n[i][0], &branchConsts, m_ctx->getProg()->getBool(), true);
      if (!condition) {
        isValid = false;
        continue;
      }
      if (applyImplicitConversion(&condition, m_ctx->getProg()->getBool(), n[i][0].getSpan())) {
        conditions.push_back(std::move(condition));
      } else {
        isValid = false;
      }
    }

    auto branch = getSubExpr(n[i][isElseClause ? 0 : 1], &branchConsts, m_typeHint);
    if (!branch) {
      isValid = false;
      continue;
    }
    branchTypes.push_back(branch->getType());
    branches.push_back(std::move(branch));
  }

  // If no preference is given over the desired output type then check what type the branches
  // have in common.
  if (m_typeHint.isInfer()) {
    auto commonType = m_ctx->getProg()->findCommonType(branchTypes);
    if (!commonType) {
      m_ctx->reportDiag(errBranchesHaveNoCommonType, n.getSpan());
      return;
    }
    m_typeHint = *commonType;
  }

  // Add a conversion for all branches that require one.
  for (auto i = 0U; i != branches.size(); ++i) {
    isValid &= applyImplicitConversion(&branches[i], m_typeHint, n[i].getSpan());
  }

  if (isValid) {
    if (!n.hasElse()) {
      if (!isExhaustive(conditions)) {
        m_ctx->reportDiag(errNonExhaustiveSwitchWithoutElse, n.getSpan());
        return;
      }
      // When we know the switch is exhaustive then we insert a 'else' branch that is never taken.
      branches.push_back(prog::expr::failNode(branches[0]->getType()));
    }
    m_expr =
        prog::expr::switchExprNode(*m_ctx->getProg(), std::move(conditions), std::move(branches));
    m_ctx->associateSrc(m_expr, n.getSpan());
  }
}

auto GetExpr::visit(const parse::UnaryExprNode& n) -> void {
  const auto& opToken = n.getOperator();
  const auto op       = getOperator(opToken);
  if (!op) {
    m_ctx->reportDiag(errUnsupportedOperator, opToken.getSpan(), opToken.str());
    return;
  }

  auto args = std::vector<prog::expr::NodePtr>{};
  args.push_back(getSubExpr(n[0], prog::sym::TypeId::inferType()));
  if (!args[0] || !args[0]->getType().isConcrete()) {
    return;
  }

  const auto argTypes  = prog::sym::TypeSet{{args[0]->getType()}};
  const auto funcName  = prog::getFuncName(op.value());
  const auto ovOptions = getOvOptions(0);
  const auto possibleFuncs =
      getFunctions(funcName, std::nullopt, argTypes, n.getOperator().getSpan(), ovOptions);
  const auto func = m_ctx->getProg()->lookupFunc(possibleFuncs, argTypes, ovOptions);
  if (!func) {
    m_ctx->reportDiag(
        errUndeclaredUnaryOperator,
        opToken.getSpan(),
        getText(op.value()),
        getDisplayName(*m_ctx, args[0]->getType()));
    return;
  }

  m_expr = prog::expr::callExprNode(*m_ctx->getProg(), func.value(), std::move(args));
  m_ctx->associateSrc(m_expr, n.getSpan());
}

auto GetExpr::visit(const parse::EnumDeclStmtNode& /*unused*/) -> void {
  throw std::logic_error{"GetExpr is not implemented for this node type"};
}

auto GetExpr::visit(const parse::ExecStmtNode& /*unused*/) -> void {
  throw std::logic_error{"GetExpr is not implemented for this node type"};
}

auto GetExpr::visit(const parse::FuncDeclStmtNode& /*unused*/) -> void {
  throw std::logic_error{"GetExpr is not implemented for this node type"};
}

auto GetExpr::visit(const parse::ImportStmtNode& /*unused*/) -> void {
  throw std::logic_error{"GetExpr is not implemented for this node type"};
}

auto GetExpr::visit(const parse::StructDeclStmtNode& /*unused*/) -> void {
  throw std::logic_error{"GetExpr is not implemented for this node type"};
}

auto GetExpr::visit(const parse::UnionDeclStmtNode& /*unused*/) -> void {
  throw std::logic_error{"GetExpr is not implemented for this node type"};
}

auto GetExpr::getVisibleConsts() -> std::vector<prog::sym::ConstId>* {
  return m_constBinder ? m_constBinder->getVisibleConsts() : nullptr;
}

auto GetExpr::getVisibleConstsCopy() -> std::vector<prog::sym::ConstId> {
  auto* visibleConsts = getVisibleConsts();
  return visibleConsts ? *visibleConsts : std::vector<prog::sym::ConstId>{};
}

auto GetExpr::getChildExprs(const parse::Node& n, unsigned int skipAmount)
    -> std::unique_ptr<ExprSetData> {
  return getChildExprs(n, nullptr, skipAmount);
}

auto GetExpr::getChildExprs(
    const parse::Node& n, const parse::Node* additionalNode, unsigned int skipAmount)
    -> std::unique_ptr<ExprSetData> {

  auto isValid  = true;
  auto argTypes = std::vector<prog::sym::TypeId>{};
  auto args     = std::vector<prog::expr::NodePtr>{};
  if (additionalNode) {
    auto arg = getSubExpr(*additionalNode, prog::sym::TypeId::inferType());
    if (arg) {
      const auto argType = arg->getType();
      if (argType.isConcrete()) {
        argTypes.push_back(argType);
        args.push_back(std::move(arg));
      } else {
        isValid = false;
      }
    }
  }
  for (auto i = skipAmount; i < n.getChildCount(); ++i) {
    auto arg = getSubExpr(n[i], prog::sym::TypeId::inferType());
    if (arg) {
      const auto argType = arg->getType();
      if (argType.isConcrete()) {
        argTypes.push_back(argType);
        args.push_back(std::move(arg));
        continue;
      }
    }
    isValid = false;
  }
  if (!isValid) {
    return nullptr;
  }

  auto argTypeSet = prog::sym::TypeSet{std::move(argTypes)};
  return std::make_unique<ExprSetData>(std::make_pair(std::move(args), std::move(argTypeSet)));
}

auto GetExpr::getSubExpr(const parse::Node& n, prog::sym::TypeId typeHint, bool checkedConstsAccess)
    -> prog::expr::NodePtr {
  // Transfer all flags to the sub-expression except for the 'CheckedConstsAccess'.
  auto subExprFlags = checkedConstsAccess ? m_flags | Flags::CheckedConstsAccess
                                          : m_flags & ~Flags::CheckedConstsAccess;

  // Pure func calls are allowed on all sub-expressions.
  subExprFlags = subExprFlags | Flags::AllowPureFuncCalls;

  auto visitor = GetExpr{m_ctx, m_typeSubTable, m_constBinder, typeHint, m_selfSig, subExprFlags};
  n.accept(&visitor);
  return std::move(visitor.getValue());
}

auto GetExpr::getSubExpr(
    const parse::Node& n,
    std::vector<prog::sym::ConstId>* visibleConsts,
    prog::sym::TypeId typeHint,
    bool checkedConstsAccess) -> prog::expr::NodePtr {

  // Transfer all flags to the sub-expression except for the 'CheckedConstsAccess'.
  auto subExprFlags = checkedConstsAccess ? m_flags | Flags::CheckedConstsAccess
                                          : m_flags & ~Flags::CheckedConstsAccess;

  // Pure func calls are allowed on all sub-expressions.
  subExprFlags = subExprFlags | Flags::AllowPureFuncCalls;

  // Override the visible constants for this sub-expression.
  auto* orgVisibleConsts = getVisibleConsts();
  if (m_constBinder) {
    m_constBinder->setVisibleConsts(visibleConsts);
  }

  auto visitor = GetExpr{m_ctx, m_typeSubTable, m_constBinder, typeHint, m_selfSig, subExprFlags};
  n.accept(&visitor);

  if (m_constBinder) {
    m_constBinder->setVisibleConsts(orgVisibleConsts);
  }
  return std::move(visitor.getValue());
}

auto GetExpr::applyImplicitConversion(
    prog::expr::NodePtr* expr, prog::sym::TypeId toType, input::Span span) -> bool {
  const auto fromType = (*expr)->getType();
  if (fromType == toType) {
    return true;
  }

  const auto conv = m_ctx->getProg()->lookupImplicitConv(fromType, toType);
  if (!conv) {
    m_ctx->reportDiag(
        errNoImplicitConversionFound,
        span,
        getDisplayName(*m_ctx, fromType),
        getDisplayName(*m_ctx, toType));
    return false;
  }

  auto convArgs = std::vector<prog::expr::NodePtr>{};
  convArgs.push_back(std::move(*expr));
  *expr = prog::expr::callExprNode(*m_ctx->getProg(), *conv, std::move(convArgs));
  return true;
}

auto GetExpr::getBinLogicOpExpr(const parse::BinaryExprNode& n, BinLogicOp op)
    -> prog::expr::NodePtr {
  auto isValid = true;

  // 'And' has checked constants, meaning the constants that are declared are only accessed
  // when the expressions evaluate to 'true'.
  auto lhsCheckedConsts = op == BinLogicOp::And;
  auto lhs              = getSubExpr(n[0], m_ctx->getProg()->getBool(), lhsCheckedConsts);

  // Rhs might not get executed, so we only expose constants when 'checkedConstsAccess' is 'true'
  // for this expression (meaning that the constants are only observed when both lfs and rhs
  // evaluate to 'true')
  auto rhsConstantsCopy = getVisibleConstsCopy();
  auto rhs              = getSubExpr(
      n[1],
      hasFlag<Flags::CheckedConstsAccess>() ? getVisibleConsts() : &rhsConstantsCopy,
      m_ctx->getProg()->getBool(),
      hasFlag<Flags::CheckedConstsAccess>() && op == BinLogicOp::And);
  if (!lhs || !rhs) {
    return nullptr;
  }

  isValid &= applyImplicitConversion(&lhs, m_ctx->getProg()->getBool(), n[0].getSpan());
  isValid &= applyImplicitConversion(&rhs, m_ctx->getProg()->getBool(), n[1].getSpan());

  if (isValid) {
    auto conditions = std::vector<prog::expr::NodePtr>{};
    conditions.push_back(std::move(lhs));

    auto branches = std::vector<prog::expr::NodePtr>{};
    switch (op) {
    case BinLogicOp::And:
      branches.push_back(std::move(rhs));
      branches.push_back(prog::expr::litBoolNode(*m_ctx->getProg(), false));
      break;
    case BinLogicOp::Or:
      branches.push_back(prog::expr::litBoolNode(*m_ctx->getProg(), true));
      branches.push_back(std::move(rhs));
    }

    return prog::expr::switchExprNode(
        *m_ctx->getProg(), std::move(conditions), std::move(branches));
  }
  return nullptr;
}

auto GetExpr::getStaticFieldExpr(
    const lex::Token& nameToken,
    const std::optional<parse::TypeParamList>& typeParams,
    const lex::Token& fieldToken) -> prog::expr::NodePtr {
  // Get the type that this field is 'on'.
  const auto type = getOrInstType(m_ctx, m_typeSubTable, nameToken, typeParams);
  if (!type) {
    assert(m_ctx->hasErrors());
    return nullptr;
  }

  const auto fieldName = getName(fieldToken);
  const auto& typeDecl = m_ctx->getProg()->getTypeDecl(*type);

  if (typeDecl.getKind() == prog::sym::TypeKind::Enum) {
    const auto& enumDef = std::get<prog::sym::EnumDef>(m_ctx->getProg()->getTypeDef(*type));
    if (!enumDef.hasEntry(fieldName)) {
      m_ctx->reportDiag(
          errValueNotFoundInEnum, fieldToken.getSpan(), fieldName, getDisplayName(*m_ctx, *type));
      return nullptr;
    }
    return prog::expr::litEnumNode(*m_ctx->getProg(), *type, fieldName);
  }

  m_ctx->reportDiag(
      errStaticFieldNotFoundOnType, fieldToken.getSpan(), fieldName, getDisplayName(*m_ctx, *type));
  return nullptr;
}

auto GetExpr::getConstExpr(const parse::IdExprNode& n) -> prog::expr::NodePtr {
  const auto name = getName(n.getId());
  if (!m_constBinder) {
    m_ctx->reportDiag(errUndeclaredConst, n.getSpan(), name);
    return nullptr;
  }

  const auto boundConst = m_constBinder->bind(name);
  if (boundConst) {
    return prog::expr::constExprNode(*m_constBinder->getConsts(), *boundConst);
  }

  if (m_constBinder->doesExistButNotVisible(name)) {
    m_ctx->reportDiag(errUninitializedConst, n.getSpan(), name);
  } else {
    m_ctx->reportDiag(errUndeclaredConst, n.getSpan(), name);
  }
  return nullptr;
}

auto GetExpr::getSelfCallExpr(const parse::CallExprNode& n) -> prog::expr::NodePtr {
  if (n.isFork()) {
    m_ctx->reportDiag(errForkedSelfCall, n.getSpan());
    return nullptr;
  }
  if (n.isLazy()) {
    m_ctx->reportDiag(errLazySelfCall, n.getSpan());
    return nullptr;
  }

  // Verify that there is a 'self' signature we can call.
  if (!m_selfSig) {
    m_ctx->reportDiag(errSelfCallInNonFunc, n.getSpan());
    return nullptr;
  }

  // Verify that the self-signature has a concrete type.
  if (!m_selfSig->second.isConcrete()) {
    m_ctx->reportDiag(errSelfCallWithoutInferredRetType, n.getSpan());
    return nullptr;
  }

  auto selfArgCount = m_selfSig->first.getCount();
  auto argCount     = n.getChildCount() - 1; // -1 because first child is the target not an arg.

  // Verify that correct amount of arguments are provided.
  if (selfArgCount != argCount) {
    m_ctx->reportDiag(errIncorrectNumArgsInSelfCall, n.getSpan(), selfArgCount, argCount);
    return nullptr;
  }

  // Analyze all arguments.
  auto args = getChildExprs(n, 1U); // Skip 1 because first child is the target not an arg.
  if (!args) {
    assert(m_ctx->hasErrors());
    return nullptr;
  }

  // Apply implicit conversions (if needed) and fail if types are incompatible.
  for (auto i = 0U; i != argCount; i++) {
    if (!applyImplicitConversion(&args->first[i], m_selfSig->first[i], n[i + 1].getSpan())) {
      return nullptr;
    }
  }

  return prog::expr::callSelfExprNode(m_selfSig->second, std::move(args->first));
}

auto GetExpr::getDynCallExpr(const parse::CallExprNode& n) -> prog::expr::NodePtr {
  auto args = getChildExprs(n);
  if (!args) {
    return nullptr;
  }

  auto ovOptions = getOvOptions(-1);

  // Check if this is a call to to a delegate type.
  if (m_ctx->getProg()->isDelegate(args->second[0])) {

    // Get the delegate arguments (skip 0 as thats the delegate itself).
    auto delArgs = std::vector<prog::expr::NodePtr>{};
    for (auto i = 1U; i != args->first.size(); ++i) {
      delArgs.push_back(std::move(args->first[i]));
    }

    // Check if we are allowed to call this delegate.
    if (!m_ctx->getProg()->satisfiesOptions(args->second[0], ovOptions)) {
      m_ctx->reportDiag(errIllegalDelegateCall, n.getSpan());
      return nullptr;
    }

    // Check if our argument types match the delegate input types.
    if (!m_ctx->getProg()->isCallable(args->second[0], getTypeSet(delArgs), ovOptions)) {
      m_ctx->reportDiag(errIncorrectArgsToDelegate, n.getSpan());
      return nullptr;
    }

    if (n.isFork() || n.isLazy()) {
      assert(n.isLazy() != n.isFork());

      return prog::expr::callDynExprNode(
          *m_ctx->getProg(),
          std::move(args->first[0]),
          n.isFork() ? *delegateOutAsFuture(m_ctx, args->second[0])
                     : *delegateOutAsLazy(m_ctx, args->second[0]),
          std::move(delArgs),
          n.isFork() ? prog::expr::CallMode::Forked : prog::expr::CallMode::Lazy);
    }

    return prog::expr::callDynExprNode(
        *m_ctx->getProg(), std::move(args->first[0]), std::move(delArgs));
  }

  // Check if this is a call to a overloaded call operator.
  const auto funcName = prog::getFuncName(prog::Operator::ParenParen);
  const auto possibleFuncs =
      getFunctions(funcName, std::nullopt, args->second, n.getSpan(), ovOptions);
  const auto func = m_ctx->getProg()->lookupFunc(possibleFuncs, args->second, ovOptions);
  if (!func) {
    auto argTypeNames = std::vector<std::string>{};
    for (const auto& argType : args->second) {
      argTypeNames.push_back(getDisplayName(*m_ctx, argType));
    }
    m_ctx->reportDiag(errUndeclaredCallOperator, n.getSpan(), argTypeNames);
    return nullptr;
  }

  if (n.isFork() || n.isLazy()) {
    assert(n.isLazy() != n.isFork());
    assert(!m_ctx->getProg()->getFuncDecl(*func).isAction()); // Call operators cannot be actions.

    return prog::expr::callExprNode(
        *m_ctx->getProg(),
        func.value(),
        n.isFork() ? funcOutAsFuture(m_ctx, *func) : funcOutAsLazy(m_ctx, *func),
        std::move(args->first),
        n.isFork() ? prog::expr::CallMode::Forked : prog::expr::CallMode::Lazy);
  }
  return prog::expr::callExprNode(*m_ctx->getProg(), func.value(), std::move(args->first));
}

auto GetExpr::declareConst(const lex::Token& nameToken, prog::sym::TypeId type)
    -> std::optional<prog::sym::ConstId> {
  if (!m_constBinder) {
    throw std::logic_error{"Constant cannot be declared in this context"};
  }

  const auto constName =
      getConstName(m_ctx, m_typeSubTable, *m_constBinder->getConsts(), nameToken);
  if (!constName) {
    return std::nullopt;
  }
  return m_constBinder->registerLocal(*constName, type);
}

auto GetExpr::isExhaustive(const std::vector<prog::expr::NodePtr>& conditions) const -> bool {
  // Note: At the moment only union exhaustiveness check is implemented.
  auto checkUnion = CheckUnionExhaustiveness{*m_ctx};
  for (const auto& cond : conditions) {
    cond->accept(&checkUnion);
  }
  return checkUnion.isExhaustive();
}

auto GetExpr::getFunctionsInclConversions(
    const lex::Token& nameToken,
    const std::optional<parse::TypeParamList>& typeParams,
    const prog::sym::TypeSet& argTypes,
    prog::OvOptions ovOptions) -> std::vector<prog::sym::FuncId> {

  const auto funcName = getName(nameToken);
  auto result  = getFunctions(funcName, typeParams, argTypes, nameToken.getSpan(), ovOptions);
  auto isValid = true;

  // Check if this name + typeParams is a type (or type template) in the program.
  auto convType = getOrInstType(m_ctx, m_typeSubTable, nameToken, typeParams, argTypes);
  if (convType) {
    // Check if there is a constructor / conversion function for this type in the program.
    const auto typeName = getName(*m_ctx, *convType);
    const auto funcs =
        m_ctx->getProg()->lookupFunc(typeName, prog::OvOptions{prog::OvFlags::ExclActions});
    result.insert(result.end(), funcs.begin(), funcs.end());

    // Check if there is a function template we can instantiate for this type.
    const auto& typeInfo = m_ctx->getTypeInfo(*convType);
    if (typeInfo && typeInfo->hasParams()) {
      const auto instantiations = m_ctx->getFuncTemplates()->instantiate(
          typeInfo->getName(), *typeInfo->getParams(), prog::OvOptions{prog::OvFlags::ExclActions});
      for (const auto& inst : instantiations) {
        if (!inst->isSuccess()) {
          m_ctx->reportDiag(errInvalidFuncInstantiation, nameToken.getSpan());
          isValid = false;
        } else {
          result.push_back(*inst->getFunc());
        }
      }
    }
  }

  return isValid ? result : std::vector<prog::sym::FuncId>{};
}

auto GetExpr::getFunctions(
    const std::string& funcName,
    const std::optional<parse::TypeParamList>& typeParams,
    const prog::sym::TypeSet& argTypes,
    input::Span span,
    prog::OvOptions ovOptions) -> std::vector<prog::sym::FuncId> {

  auto result  = std::vector<prog::sym::FuncId>{};
  auto isValid = true;

  if (typeParams) {
    // If this is a template call then instantiate the templates.
    auto typeParamSet = getTypeSet(m_ctx, m_typeSubTable, typeParams->getTypes());
    if (!typeParamSet) {
      assert(m_ctx->hasErrors());
      return {};
    }
    const auto instantiations =
        m_ctx->getFuncTemplates()->instantiate(funcName, *typeParamSet, ovOptions);
    for (const auto& inst : instantiations) {
      if (!inst->isSuccess()) {
        m_ctx->reportDiag(errInvalidFuncInstantiation, span);
        isValid = false;
      } else {
        result.push_back(*inst->getFunc());
      }
    }
  } else { // no type params.

    // Find all non-templated funcs.
    result = m_ctx->getProg()->lookupFunc(funcName, ovOptions);

    // If we found a match, then there is no need to attempt to instantiate a template.
    if (m_ctx->getProg()->lookupFunc(result, argTypes, ovOptions)) {
      return result;
    }

    // Find templated funcs where we can infer the type params based on the argument types.
    const auto instantiations =
        m_ctx->getFuncTemplates()->inferParamsAndInstantiate(funcName, argTypes, ovOptions);
    for (const auto& inst : instantiations) {
      if (!inst->isSuccess()) {
        m_ctx->reportDiag(errInvalidFuncInstantiation, span);
        isValid = false;
      } else {
        result.push_back(*inst->getFunc());
      }
    }
  }

  return isValid ? result : std::vector<prog::sym::FuncId>{};
}

} // namespace frontend::internal
