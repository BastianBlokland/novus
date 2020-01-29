#include "internal/get_expr.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/check_union_exhaustiveness.hpp"
#include "internal/define_user_funcs.hpp"
#include "internal/get_identifier.hpp"
#include "internal/utilities.hpp"
#include "lex/token_payload_lit_bool.hpp"
#include "lex/token_payload_lit_char.hpp"
#include "lex/token_payload_lit_float.hpp"
#include "lex/token_payload_lit_int.hpp"
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
    Flags flags) :
    m_ctx{ctx},
    m_typeSubTable{typeSubTable},
    m_constBinder{constBinder},
    m_typeHint{typeHint},
    m_flags{flags} {

  if (m_ctx == nullptr) {
    throw std::invalid_argument{"Context cannot be null"};
  }
  if (m_constBinder == nullptr) {
    throw std::invalid_argument{"Constant binder cannot be null"};
  }
}

auto GetExpr::getValue() -> prog::expr::NodePtr& { return m_expr; }

auto GetExpr::visit(const parse::CommentNode & /*unused*/) -> void {
  throw std::logic_error{"GetExpr is not implemented for this node type"};
}

auto GetExpr::visit(const parse::ErrorNode & /*unused*/) -> void {
  throw std::logic_error{"GetExpr is not implemented for this node type"};
}

auto GetExpr::visit(const parse::AnonFuncExprNode& n) -> void {
  auto consts = prog::sym::ConstDeclTable{};
  if (!declareFuncInput(m_ctx, m_typeSubTable, n, &consts)) {
    assert(m_ctx->hasErrors());
    return;
  }

  // Analyze the body of the anonymous function.
  auto visibleConsts = consts.getAll();
  auto constBinder   = ConstBinder{&consts, &visibleConsts, m_constBinder};
  auto getExprFlags  = Flags::AllowPureFuncCalls;
  auto getExpr =
      GetExpr{m_ctx, m_typeSubTable, &constBinder, prog::sym::TypeId::inferType(), getExprFlags};
  n[0].accept(&getExpr);
  if (!getExpr.m_expr) {
    assert(m_ctx->hasErrors());
    return;
  }

  // Gather the input types for the function.
  auto inputTypes = std::vector<prog::sym::TypeId>{};
  for (const auto constId : consts.getInputs()) {
    inputTypes.push_back(consts[constId].getType());
  }
  auto inputTypeSet = prog::sym::TypeSet{std::move(inputTypes)};

  // Generate a unique function name.
  std::ostringstream nameoss;
  nameoss << "__anon_" << m_ctx->getProg()->getFuncCount();

  // Declare and define the function in the program.
  const auto funcId = m_ctx->getProg()->declarePureFunc(
      nameoss.str(), std::move(inputTypeSet), getExpr.m_expr->getType());
  m_ctx->getProg()->defineFunc(funcId, std::move(consts), std::move(getExpr.m_expr));

  // Either create a function literal or a closure, depending on if the anonymous func binds any
  // consts from the parent.
  if (constBinder.getBoundParentConsts().empty()) {
    m_expr = getLitFunc(m_ctx, funcId);
  } else {
    auto boundArgs = std::vector<prog::expr::NodePtr>{};
    for (const auto parentConstId : constBinder.getBoundParentConsts()) {
      boundArgs.push_back(prog::expr::constExprNode(*m_constBinder->getConsts(), parentConstId));
    }
    m_expr = getFuncClosure(m_ctx, funcId, std::move(boundArgs));
  }
}

auto GetExpr::visit(const parse::BinaryExprNode& n) -> void {
  const auto& opToken = n.getOperator();
  switch (opToken.getKind()) {
  case lex::TokenKind::OpAmpAmp:
    m_expr = getBinLogicOpExpr(n, BinLogicOp::And);
    return;
  case lex::TokenKind::OpPipePipe:
    m_expr = getBinLogicOpExpr(n, BinLogicOp::Or);
    return;
  default:
    break;
  }
  const auto op = getOperator(opToken);
  if (!op) {
    m_ctx->reportDiag(errUnsupportedOperator, opToken.str(), opToken.getSpan());
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
  const auto funcName   = prog::getFuncName(op.value());
  const auto possibleFuncs =
      getFunctions(funcName, std::nullopt, argTypeSet, n.getOperator().getSpan(), true);
  const auto func = m_ctx->getProg()->lookupFunc(possibleFuncs, argTypeSet, getOvOptions(1));
  if (!func) {
    m_ctx->reportDiag(
        errUndeclaredBinOperator,
        getText(op.value()),
        getDisplayName(*m_ctx, args[0]->getType()),
        getDisplayName(*m_ctx, args[1]->getType()),
        opToken.getSpan());
    return;
  }

  m_expr = prog::expr::callExprNode(*m_ctx->getProg(), func.value(), std::move(args));
}

auto GetExpr::visit(const parse::CallExprNode& n) -> void {
  auto getIdVisitor = GetIdentifier{true};
  n[0].accept(&getIdVisitor);
  auto* instance  = getIdVisitor.getInstance();
  auto identifier = getIdVisitor.getIdentifier();
  auto typeParams = getIdVisitor.getTypeParams();

  /* Check what kind of call this is.
  - If lhs is not an identifier (but some other expression): Dynamic call.
  - Lhs is an identifier that matches a constant name: Dynamic call.
  - Lhs has an instance (like a struct) and its (field) name is not a function: Dynamic call.
  - Otherwise: Static call (If there is an lhs instance its treated as the first arg). */
  if (!identifier || m_constBinder->canBind(getName(*identifier)) ||
      (instance != nullptr && !isFuncOrConv(m_ctx, getName(*identifier)))) {

    m_expr = getDynCallExpr(n);
    return;
  }

  auto nameToken = *identifier;
  auto args      = getChildExprs(n, instance, 1);
  if (!args) {
    assert(m_ctx->hasErrors());
    return;
  }

  // Assert has an exception where you are allowed omit the second arg (the message).
  // TODO(Bastian): Create a general mechanism for these kinds of exceptions.
  if (getName(nameToken) == "assert" && args->second.getCount() == 1 &&
      args->second[0] == m_ctx->getProg()->getBool()) {
    std::ostringstream msgoss;
    msgoss << m_ctx->getSrc().getId() << " " << m_ctx->getSrc().getTextPos(n.getSpan().getStart());
    args->first.push_back(prog::expr::litStringNode(*m_ctx->getProg(), msgoss.str()));
    args->second = args->second.withExtraType(m_ctx->getProg()->getString());
  }

  // Actions cannot be called using the 'instance call' syntax.
  const auto exclActions = instance != nullptr;

  const auto possibleFuncs =
      getFunctionsInclConversions(nameToken, typeParams, args->second, exclActions);
  const auto func = m_ctx->getProg()->lookupFunc(possibleFuncs, args->second, getOvOptions(-1));
  if (!func) {
    auto isTypeOrConv = isType(m_ctx, getName(nameToken));
    if (typeParams) {
      if (isTypeOrConv) {
        m_ctx->reportDiag(
            errNoTypeOrConversionFoundToInstantiate,
            getName(nameToken),
            typeParams->getCount(),
            n.getSpan());
      } else {
        m_ctx->reportDiag(
            errNoFuncFoundToInstantiate, getName(nameToken), typeParams->getCount(), n.getSpan());
      }
    } else {
      auto argTypeNames = std::vector<std::string>{};
      for (const auto& argType : args->second) {
        argTypeNames.push_back(getDisplayName(*m_ctx, argType));
      }
      if (isTypeOrConv) {
        m_ctx->reportDiag(
            errUndeclaredTypeOrConversion, getName(nameToken), argTypeNames, n.getSpan());
      } else {
        if (hasFlag<Flags::AllowPureFuncCalls>() && hasFlag<Flags::AllowActionCalls>() &&
            !exclActions) {
          m_ctx->reportDiag(
              errUndeclaredFuncOrAction, getName(nameToken), argTypeNames, n.getSpan());
        } else if (hasFlag<Flags::AllowPureFuncCalls>()) {
          m_ctx->reportDiag(errUndeclaredPureFunc, getName(nameToken), argTypeNames, n.getSpan());
        } else {
          m_ctx->reportDiag(errUndeclaredAction, getName(nameToken), argTypeNames, n.getSpan());
        }
      }
    }
    return;
  }
  m_expr = prog::expr::callExprNode(*m_ctx->getProg(), func.value(), std::move(args->first));
}

auto GetExpr::visit(const parse::ConditionalExprNode& n) -> void {
  auto conditions = std::vector<prog::expr::NodePtr>{};
  auto branches   = std::vector<prog::expr::NodePtr>{};
  conditions.push_back(getSubExpr(n[0], m_ctx->getProg()->getBool(), true));
  if (!conditions[0]) {
    return;
  }
  if (!applyImplicitConversion(&conditions[0], m_ctx->getProg()->getBool(), n[0].getSpan())) {
    return;
  }

  // Make a copy of the visible constants, constants should not be visible outside.
  auto ifBranchConsts = *m_constBinder->getVisibleConsts();
  branches.push_back(getSubExpr(n[1], &ifBranchConsts, m_typeHint));

  // Make a copy of the visible constants, constants should not be visible outside.
  auto elseBranchConsts = *m_constBinder->getVisibleConsts();
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
}

auto GetExpr::visit(const parse::ConstDeclExprNode& n) -> void {
  auto assignExpr = getSubExpr(n[0], prog::sym::TypeId::inferType());
  if (assignExpr == nullptr) {
    return;
  }
  const auto constId = declareConst(n.getId(), assignExpr->getType());
  if (constId) {
    m_expr = prog::expr::assignExprNode(
        *m_constBinder->getConsts(), constId.value(), std::move(assignExpr));
  }
}

auto GetExpr::visit(const parse::IdExprNode& n) -> void {
  const auto name = getName(n.getId());
  // Check if this is a constant, if so bind to it.
  if (m_constBinder->canBind(name)) {
    m_expr = getConstExpr(n);
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
        name, *typeSet, prog::OvOptions{prog::OvFlags::ExclActions});
    if (instances.empty()) {
      m_ctx->reportDiag(errNoFuncFoundToInstantiate, name, typeSet->getCount(), n.getSpan());
      return;
    }
    if (instances.size() != 1) {
      m_ctx->reportDiag(errAmbiguousTemplateFunction, name, typeSet->getCount(), n.getSpan());
      return;
    }
    m_expr = getLitFunc(m_ctx, *instances[0]->getFunc());
    return;
  }

  // Non-templated function literal.
  const auto funcs =
      m_ctx->getProg()->lookupFuncs(name, prog::OvOptions{prog::OvFlags::ExclActions});
  if (!funcs.empty()) {
    if (funcs.size() != 1) {
      m_ctx->reportDiag(errAmbiguousFunction, name, n.getSpan());
      return;
    }
    m_expr = getLitFunc(m_ctx, funcs[0]);
    return;
  }

  // Error if no templated args are provided to templated function.
  if (m_ctx->getFuncTemplates()->hasFunc(name)) {
    m_ctx->reportDiag(errNoTypeParamsProvidedToTemplateFunction, name, n.getSpan());
    return;
  }

  // If its not a function literal treat it as a constant, even tho we already checked if this is
  // a constant at the top of this function we would rather have the error be 'no constant found'
  // then no function found.
  m_expr = getConstExpr(n);
  assert(m_expr != nullptr || m_ctx->hasErrors());
}

auto GetExpr::visit(const parse::FieldExprNode& n) -> void {
  // Check if the lhs is a type name token, in that case treat this as access to a 'static' field.
  auto getIdVisitor = GetIdentifier{false};
  n[0].accept(&getIdVisitor);
  auto identifier = getIdVisitor.getIdentifier();
  if (identifier && isType(m_ctx, getName(*identifier))) {
    m_expr = getStaticFieldExpr(*identifier, getIdVisitor.getTypeParams(), n.getId());
    return;
  }

  // If not a static field then acces the field on the lhs expression.
  auto lhsExpr = getSubExpr(n[0], prog::sym::TypeId::inferType());
  if (lhsExpr == nullptr) {
    return;
  }

  const auto fieldName    = getName(n.getId());
  const auto lhsType      = lhsExpr->getType();
  const auto& lhsTypeDecl = m_ctx->getProg()->getTypeDecl(lhsType);
  if (lhsTypeDecl.getKind() != prog::sym::TypeKind::Struct) {
    m_ctx->reportDiag(
        errFieldNotFoundOnType, fieldName, getDisplayName(*m_ctx, lhsType), n.getSpan());
    return;
  }
  const auto& structDef = std::get<prog::sym::StructDef>(m_ctx->getProg()->getTypeDef(lhsType));
  const auto field      = structDef.getFields().lookup(fieldName);
  if (!field) {
    m_ctx->reportDiag(
        errFieldNotFoundOnType, fieldName, getDisplayName(*m_ctx, lhsType), n.getSpan());
    return;
  }

  m_expr = prog::expr::fieldExprNode(*m_ctx->getProg(), std::move(lhsExpr), *field);
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
  }
}

auto GetExpr::visit(const parse::IndexExprNode& n) -> void {
  auto args = getChildExprs(n);
  if (!args) {
    assert(m_ctx->hasErrors());
    return;
  }

  const auto funcName      = prog::getFuncName(prog::Operator::SquareSquare);
  const auto possibleFuncs = getFunctions(funcName, std::nullopt, args->second, n.getSpan(), true);
  const auto func = m_ctx->getProg()->lookupFunc(possibleFuncs, args->second, getOvOptions(-1));
  if (!func) {
    auto argTypeNames = std::vector<std::string>{};
    for (const auto& argType : args->second) {
      argTypeNames.push_back(getDisplayName(*m_ctx, argType));
    }
    m_ctx->reportDiag(errUndeclaredIndexOperator, argTypeNames, n.getSpan());
    return;
  }

  m_expr = prog::expr::callExprNode(*m_ctx->getProg(), func.value(), std::move(args->first));
}

auto GetExpr::visit(const parse::IsExprNode& n) -> void {
  auto lhsExpr = getSubExpr(n[0], prog::sym::TypeId::inferType());
  if (lhsExpr == nullptr) {
    return;
  }

  // Validate lhs is a union type.
  const auto lhsType      = lhsExpr->getType();
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
        errUndeclaredType, getName(parseType), parseType.getParamCount(), parseType.getSpan());
    return;
  }

  // Validate that the type is part of the union.
  if (!unionDef.hasType(*type)) {
    m_ctx->reportDiag(
        errTypeNotPartOfUnion,
        getDisplayName(*m_ctx, *type),
        getDisplayName(*m_ctx, lhsType),
        parseType.getSpan());
    return;
  }

  if (!n.hasId()) {
    m_expr = prog::expr::unionCheckExprNode(*m_ctx->getProg(), std::move(lhsExpr), *type);
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
  }
}

auto GetExpr::visit(const parse::LitExprNode& n) -> void {
  switch (n.getVal().getKind()) {
  case lex::TokenKind::LitInt:
    m_expr = prog::expr::litIntNode(
        *m_ctx->getProg(), n.getVal().getPayload<lex::LitIntTokenPayload>()->getValue());
    return;
  case lex::TokenKind::LitFloat:
    m_expr = prog::expr::litFloatNode(
        *m_ctx->getProg(), n.getVal().getPayload<lex::LitFloatTokenPayload>()->getValue());
    return;
  case lex::TokenKind::LitBool:
    m_expr = prog::expr::litBoolNode(
        *m_ctx->getProg(), n.getVal().getPayload<lex::LitBoolTokenPayload>()->getValue());
    return;
  case lex::TokenKind::LitString:
    m_expr = prog::expr::litStringNode(
        *m_ctx->getProg(), n.getVal().getPayload<lex::LitStringTokenPayload>()->getValue());
    return;
  case lex::TokenKind::LitChar:
    m_expr = prog::expr::litCharNode(
        *m_ctx->getProg(), n.getVal().getPayload<lex::LitCharTokenPayload>()->getValue());
    return;
  default:
    std::stringstream oss;
    oss << n.getVal().getKind();
    m_ctx->reportDiag(errUnsupportedLiteral, oss.str(), n.getSpan());
  }
}

auto GetExpr::visit(const parse::ParenExprNode& n) -> void {
  m_expr = getSubExpr(n[0], m_typeHint, hasFlag<Flags::CheckedConstsAccess>());
}

auto GetExpr::visit(const parse::SwitchExprElseNode & /*unused*/) -> void {
  throw std::logic_error{"GetExpr is not implemented for this node type"};
}

auto GetExpr::visit(const parse::SwitchExprIfNode & /*unused*/) -> void {
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
    auto branchConsts = *m_constBinder->getVisibleConsts();
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
  }
}

auto GetExpr::visit(const parse::UnaryExprNode& n) -> void {
  const auto& opToken = n.getOperator();
  const auto op       = getOperator(opToken);
  if (!op) {
    m_ctx->reportDiag(errUnsupportedOperator, opToken.str(), opToken.getSpan());
    return;
  }

  auto args = std::vector<prog::expr::NodePtr>{};
  args.push_back(getSubExpr(n[0], prog::sym::TypeId::inferType()));
  if (!args[0] || !args[0]->getType().isConcrete()) {
    return;
  }

  const auto argTypes = prog::sym::TypeSet{{args[0]->getType()}};
  const auto funcName = prog::getFuncName(op.value());
  const auto possibleFuncs =
      getFunctions(funcName, std::nullopt, argTypes, n.getOperator().getSpan(), true);
  const auto func = m_ctx->getProg()->lookupFunc(possibleFuncs, argTypes, getOvOptions(0));
  if (!func) {
    m_ctx->reportDiag(
        errUndeclaredUnaryOperator,
        getText(op.value()),
        getDisplayName(*m_ctx, args[0]->getType()),
        opToken.getSpan());
    return;
  }

  m_expr = prog::expr::callExprNode(*m_ctx->getProg(), func.value(), std::move(args));
}

auto GetExpr::visit(const parse::EnumDeclStmtNode & /*unused*/) -> void {
  throw std::logic_error{"GetExpr is not implemented for this node type"};
}

auto GetExpr::visit(const parse::ExecStmtNode & /*unused*/) -> void {
  throw std::logic_error{"GetExpr is not implemented for this node type"};
}

auto GetExpr::visit(const parse::FuncDeclStmtNode & /*unused*/) -> void {
  throw std::logic_error{"GetExpr is not implemented for this node type"};
}

auto GetExpr::visit(const parse::ImportStmtNode & /*unused*/) -> void {
  throw std::logic_error{"GetExpr is not implemented for this node type"};
}

auto GetExpr::visit(const parse::StructDeclStmtNode & /*unused*/) -> void {
  throw std::logic_error{"GetExpr is not implemented for this node type"};
}

auto GetExpr::visit(const parse::UnionDeclStmtNode & /*unused*/) -> void {
  throw std::logic_error{"GetExpr is not implemented for this node type"};
}

auto GetExpr::getChildExprs(const parse::Node& n, unsigned int skipAmount)
    -> std::optional<std::pair<std::vector<prog::expr::NodePtr>, prog::sym::TypeSet>> {
  return getChildExprs(n, nullptr, skipAmount);
}

auto GetExpr::getChildExprs(
    const parse::Node& n, const parse::Node* additionalNode, unsigned int skipAmount)
    -> std::optional<std::pair<std::vector<prog::expr::NodePtr>, prog::sym::TypeSet>> {
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
    return std::nullopt;
  }

  auto argTypeSet = prog::sym::TypeSet{std::move(argTypes)};
  return std::make_pair(std::move(args), std::move(argTypeSet));
}

auto GetExpr::getSubExpr(const parse::Node& n, prog::sym::TypeId typeHint, bool checkedConstsAccess)
    -> prog::expr::NodePtr {
  // Transfer all flags to the sub-expression except for the 'CheckedConstsAccess'/
  auto subExprFlags = checkedConstsAccess ? m_flags | Flags::CheckedConstsAccess
                                          : m_flags & ~Flags::CheckedConstsAccess;

  // Pure func calls are allowed on all sub-expressions.
  subExprFlags = subExprFlags | Flags::AllowPureFuncCalls;

  auto visitor = GetExpr{m_ctx, m_typeSubTable, m_constBinder, typeHint, subExprFlags};
  n.accept(&visitor);
  return std::move(visitor.getValue());
}

auto GetExpr::getSubExpr(
    const parse::Node& n,
    std::vector<prog::sym::ConstId>* visibleConsts,
    prog::sym::TypeId typeHint,
    bool checkedConstsAccess) -> prog::expr::NodePtr {

  // Transfer all flags to the sub-expression except for the 'CheckedConstsAccess'/
  auto subExprFlags = checkedConstsAccess ? m_flags | Flags::CheckedConstsAccess
                                          : m_flags & ~Flags::CheckedConstsAccess;

  // Pure func calls are allowed on all sub-expressions.
  subExprFlags = subExprFlags | Flags::AllowPureFuncCalls;

  // Override the visible constants for this sub-expression.
  auto* orgVisibleConsts = m_constBinder->getVisibleConsts();
  m_constBinder->setVisibleConsts(visibleConsts);

  auto visitor = GetExpr{m_ctx, m_typeSubTable, m_constBinder, typeHint, subExprFlags};
  n.accept(&visitor);

  m_constBinder->setVisibleConsts(orgVisibleConsts);
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
        getDisplayName(*m_ctx, fromType),
        getDisplayName(*m_ctx, toType),
        span);
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
  auto rhsConstantsCopy = hasFlag<Flags::CheckedConstsAccess>()
      ? nullptr
      : std::make_unique<std::vector<prog::sym::ConstId>>(*m_constBinder->getVisibleConsts());
  auto rhs = getSubExpr(
      n[1],
      rhsConstantsCopy ? rhsConstantsCopy.get() : m_constBinder->getVisibleConsts(),
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
          errValueNotFoundInEnum, fieldName, getDisplayName(*m_ctx, *type), fieldToken.getSpan());
      return nullptr;
    }
    return prog::expr::litEnumNode(*m_ctx->getProg(), *type, fieldName);
  }

  m_ctx->reportDiag(
      errStaticFieldNotFoundOnType, fieldName, getDisplayName(*m_ctx, *type), fieldToken.getSpan());
  return nullptr;
}

auto GetExpr::getConstExpr(const parse::IdExprNode& n) -> prog::expr::NodePtr {
  const auto name       = getName(n.getId());
  const auto boundConst = m_constBinder->bind(name);
  if (boundConst) {
    return prog::expr::constExprNode(*m_constBinder->getConsts(), *boundConst);
  }

  if (m_constBinder->doesExistButNotVisible(name)) {
    m_ctx->reportDiag(errUninitializedConst, name, n.getSpan());
  } else {
    m_ctx->reportDiag(errUndeclaredConst, name, n.getSpan());
  }
  return nullptr;
}

auto GetExpr::getDynCallExpr(const parse::CallExprNode& n) -> prog::expr::NodePtr {
  auto args = getChildExprs(n);
  if (!args) {
    assert(m_ctx->hasErrors());
    return nullptr;
  }

  // Check if this is a call to to a delegate type.
  if (m_ctx->getProg()->isDelegate(args->second[0])) {
    auto delArgs = std::vector<prog::expr::NodePtr>{};
    for (auto i = 1U; i != args->first.size(); ++i) {
      delArgs.push_back(std::move(args->first[i]));
    }
    if (!m_ctx->getProg()->isCallable(args->second[0], delArgs)) {
      m_ctx->reportDiag(errIncorrectArgsToDelegate, n.getSpan());
      return nullptr;
    }
    return prog::expr::callDynExprNode(
        *m_ctx->getProg(), std::move(args->first[0]), std::move(delArgs));
  }

  // Check if this is a call to a overloaded call operator.
  const auto funcName      = prog::getFuncName(prog::Operator::ParenParen);
  const auto possibleFuncs = getFunctions(funcName, std::nullopt, args->second, n.getSpan(), true);
  const auto func = m_ctx->getProg()->lookupFunc(possibleFuncs, args->second, getOvOptions(-1));
  if (!func) {
    auto argTypeNames = std::vector<std::string>{};
    for (const auto& argType : args->second) {
      argTypeNames.push_back(getDisplayName(*m_ctx, argType));
    }
    m_ctx->reportDiag(errUndeclaredCallOperator, argTypeNames, n.getSpan());
    return nullptr;
  }

  return prog::expr::callExprNode(*m_ctx->getProg(), func.value(), std::move(args->first));
}

auto GetExpr::declareConst(const lex::Token& nameToken, prog::sym::TypeId type)
    -> std::optional<prog::sym::ConstId> {
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
    bool exclActions) -> std::vector<prog::sym::FuncId> {

  const auto funcName = getName(nameToken);
  auto result  = getFunctions(funcName, typeParams, argTypes, nameToken.getSpan(), exclActions);
  auto isValid = true;

  // Check if this name + typeParams is a type (or type template) in the program.
  auto convType = getOrInstType(m_ctx, m_typeSubTable, nameToken, typeParams, argTypes);
  if (convType) {
    // Check if there is a constructor / conversion function for this type in the program.
    const auto typeName = getName(*m_ctx, *convType);
    const auto funcs =
        m_ctx->getProg()->lookupFuncs(typeName, prog::OvOptions{prog::OvFlags::ExclActions});
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
    bool exclActions) -> std::vector<prog::sym::FuncId> {

  auto result    = std::vector<prog::sym::FuncId>{};
  auto isValid   = true;
  auto ovOptions = getOvOptions(-1, exclActions);

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
    for (const auto& f : m_ctx->getProg()->lookupFuncs(funcName, ovOptions)) {
      result.push_back(f);
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
