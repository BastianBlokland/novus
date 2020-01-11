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
#include <sstream>

namespace frontend::internal {

GetExpr::GetExpr(
    Context* context,
    const TypeSubstitutionTable* typeSubTable,
    ConstBinder* constBinder,
    prog::sym::TypeId typeHint,
    bool checkedConstsAccess) :
    m_context{context},
    m_typeSubTable{typeSubTable},
    m_constBinder{constBinder},
    m_typeHint{typeHint},
    m_checkedConstsAccess{checkedConstsAccess} {

  if (m_context == nullptr) {
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
  if (!declareFuncInput(m_context, m_typeSubTable, n, &consts)) {
    assert(m_context->hasErrors());
    return;
  }

  // Analyze the body of the anonymous function.
  auto visibleConsts = consts.getAll();
  auto constBinder   = ConstBinder{&consts, &visibleConsts, m_constBinder};
  auto getExpr = GetExpr{m_context, m_typeSubTable, &constBinder, prog::sym::TypeId::inferType()};
  n[0].accept(&getExpr);
  if (!getExpr.m_expr) {
    assert(m_context->hasErrors());
    return;
  }

  // Gather the input types for the function.
  auto inputTypes = std::vector<prog::sym::TypeId>{};
  for (const auto constId : consts.getInputs()) {
    inputTypes.push_back(consts[constId].getType());
  }
  auto inputTypeSet = prog::sym::TypeSet{std::move(inputTypes)};

  // Declare and define the function in the program.
  const auto funcId = m_context->getProg()->declareUserFunc(
      m_context->genAnonFuncName(), std::move(inputTypeSet), getExpr.m_expr->getType());
  m_context->getProg()->defineUserFunc(funcId, std::move(consts), std::move(getExpr.m_expr));

  // Either create a function literal or a closure, depending on if the anonymous func binds any
  // consts from the parent.
  if (constBinder.getBoundParentConsts().empty()) {
    m_expr = getLitFunc(m_context, funcId);
  } else {
    auto boundArgs = std::vector<prog::expr::NodePtr>{};
    for (const auto parentConstId : constBinder.getBoundParentConsts()) {
      boundArgs.push_back(prog::expr::constExprNode(*m_constBinder->getConsts(), parentConstId));
    }
    m_expr = getFuncClosure(m_context, funcId, std::move(boundArgs));
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
    m_context->reportDiag(
        errUnsupportedOperator(m_context->getSrc(), opToken.str(), opToken.getSpan()));
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
      getFunctions(funcName, std::nullopt, argTypeSet, n.getOperator().getSpan());
  const auto func = m_context->getProg()->lookupFunc(possibleFuncs, argTypeSet, 1);
  if (!func) {
    m_context->reportDiag(errUndeclaredBinOperator(
        m_context->getSrc(),
        getText(op.value()),
        getDisplayName(*m_context, args[0]->getType()),
        getDisplayName(*m_context, args[1]->getType()),
        opToken.getSpan()));
    return;
  }

  m_expr = prog::expr::callExprNode(*m_context->getProg(), func.value(), std::move(args));
}

auto GetExpr::visit(const parse::CallExprNode& n) -> void {
  auto getIdVisitor = GetIdentifier{};
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
      (instance != nullptr && !isFuncOrConv(m_context, getName(*identifier)))) {

    m_expr = getDynCallExpr(n);
    return;
  }

  auto nameToken = *identifier;
  auto args      = getChildExprs(n, instance, 1);
  if (!args) {
    assert(m_context->hasErrors());
    return;
  }

  const auto possibleFuncs = getFunctionsInclConversions(nameToken, typeParams, args->second);
  const auto func          = m_context->getProg()->lookupFunc(possibleFuncs, args->second, -1);
  if (!func) {
    auto isTypeOrConv = isType(m_context, getName(nameToken));
    if (typeParams) {
      if (isTypeOrConv) {
        m_context->reportDiag(errNoTypeOrConversionFoundToInstantiate(
            m_context->getSrc(), getName(nameToken), typeParams->getCount(), n.getSpan()));
      } else {
        m_context->reportDiag(errNoFuncFoundToInstantiate(
            m_context->getSrc(), getName(nameToken), typeParams->getCount(), n.getSpan()));
      }
    } else {
      auto argTypeNames = std::vector<std::string>{};
      for (const auto& argType : args->second) {
        argTypeNames.push_back(getDisplayName(*m_context, argType));
      }
      if (isTypeOrConv) {
        m_context->reportDiag(errUndeclaredTypeOrConversion(
            m_context->getSrc(), getName(nameToken), argTypeNames, n.getSpan()));
      } else {
        m_context->reportDiag(
            errUndeclaredFunc(m_context->getSrc(), getName(nameToken), argTypeNames, n.getSpan()));
      }
    }
    return;
  }
  m_expr = prog::expr::callExprNode(*m_context->getProg(), func.value(), std::move(args->first));
}

auto GetExpr::visit(const parse::ConditionalExprNode& n) -> void {
  auto conditions = std::vector<prog::expr::NodePtr>{};
  auto branches   = std::vector<prog::expr::NodePtr>{};
  conditions.push_back(getSubExpr(n[0], m_context->getProg()->getBool(), true));
  if (!conditions[0]) {
    return;
  }
  if (!applyConversion(&conditions[0], m_context->getProg()->getBool(), n[0].getSpan())) {
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
    auto commonType        = m_context->getProg()->findCommonType(branchTypes);
    if (!commonType) {
      m_context->reportDiag(errBranchesHaveNoCommonType(m_context->getSrc(), n.getSpan()));
      return;
    }
    m_typeHint = *commonType;
  }

  // Add a conversion for all branches that require one.
  for (auto i = 0U; i != branches.size(); ++i) {
    if (!applyConversion(&branches[i], m_typeHint, n[i].getSpan())) {
      return;
    }
  }

  m_expr =
      prog::expr::switchExprNode(*m_context->getProg(), std::move(conditions), std::move(branches));
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

  // Templated function literal.
  if (n.getTypeParams()) {
    const auto typeSet = getTypeSet(m_context, m_typeSubTable, n.getTypeParams()->getTypes());
    if (!typeSet) {
      assert(m_context->hasErrors());
      return;
    }
    const auto instances = m_context->getFuncTemplates()->instantiate(name, *typeSet);
    if (instances.empty()) {
      m_context->reportDiag(
          errNoFuncFoundToInstantiate(m_context->getSrc(), name, typeSet->getCount(), n.getSpan()));
      return;
    }
    if (instances.size() != 1) {
      m_context->reportDiag(errAmbiguousTemplateFunction(
          m_context->getSrc(), name, typeSet->getCount(), n.getSpan()));
      return;
    }
    m_expr = getLitFunc(m_context, *instances[0]->getFunc());
    return;
  }

  // Non-templated function literal.
  const auto funcs = m_context->getProg()->lookupFuncs(name);
  if (!funcs.empty()) {
    if (funcs.size() != 1) {
      m_context->reportDiag(errAmbiguousFunction(m_context->getSrc(), name, n.getSpan()));
      return;
    }
    m_expr = getLitFunc(m_context, funcs[0]);
    return;
  }

  // Error if no templated args are provided to templated function.
  if (m_context->getFuncTemplates()->hasFunc(name)) {
    m_context->reportDiag(
        errNoTypeParamsProvidedToTemplateFunction(m_context->getSrc(), name, n.getSpan()));
    return;
  }

  // If its not a function literal treat it as a constant.
  m_expr = getConstExpr(n);
  assert(m_expr != nullptr || m_context->hasErrors());
}

auto GetExpr::visit(const parse::FieldExprNode& n) -> void {
  const auto fieldName = getName(n.getId());
  auto lhsExpr         = getSubExpr(n[0], prog::sym::TypeId::inferType());
  if (lhsExpr == nullptr) {
    return;
  }

  const auto lhsType      = lhsExpr->getType();
  const auto& lhsTypeDecl = m_context->getProg()->getTypeDecl(lhsType);
  if (lhsTypeDecl.getKind() != prog::sym::TypeKind::UserStruct) {
    m_context->reportDiag(errFieldNotFoundOnType(
        m_context->getSrc(), fieldName, getDisplayName(*m_context, lhsType), n.getSpan()));
    return;
  }
  const auto& structDef = std::get<prog::sym::StructDef>(m_context->getProg()->getTypeDef(lhsType));
  const auto field      = structDef.getFields().lookup(fieldName);
  if (!field) {
    m_context->reportDiag(errFieldNotFoundOnType(
        m_context->getSrc(), fieldName, getDisplayName(*m_context, lhsType), n.getSpan()));
    return;
  }

  m_expr = prog::expr::fieldExprNode(*m_context->getProg(), std::move(lhsExpr), *field);
}

auto GetExpr::visit(const parse::GroupExprNode& n) -> void {
  auto subExprs = std::vector<prog::expr::NodePtr>();
  for (auto i = 0U; i < n.getChildCount(); ++i) {
    auto last    = i == n.getChildCount() - 1;
    auto subExpr = getSubExpr(
        n[i], last ? m_typeHint : prog::sym::TypeId::inferType(), m_checkedConstsAccess && last);
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
    assert(m_context->hasErrors());
    return;
  }

  const auto funcName      = prog::getFuncName(prog::Operator::SquareSquare);
  const auto possibleFuncs = getFunctions(funcName, std::nullopt, args->second, n.getSpan());
  const auto func          = m_context->getProg()->lookupFunc(possibleFuncs, args->second, -1);
  if (!func) {
    auto argTypeNames = std::vector<std::string>{};
    for (const auto& argType : args->second) {
      argTypeNames.push_back(getDisplayName(*m_context, argType));
    }
    m_context->reportDiag(
        errUndeclaredIndexOperator(m_context->getSrc(), argTypeNames, n.getSpan()));
    return;
  }

  m_expr = prog::expr::callExprNode(*m_context->getProg(), func.value(), std::move(args->first));
}

auto GetExpr::visit(const parse::IsExprNode& n) -> void {
  auto lhsExpr = getSubExpr(n[0], prog::sym::TypeId::inferType());
  if (lhsExpr == nullptr) {
    return;
  }

  // Validate lhs is a union type.
  const auto lhsType      = lhsExpr->getType();
  const auto& lhsTypeDecl = m_context->getProg()->getTypeDecl(lhsType);
  if (lhsTypeDecl.getKind() != prog::sym::TypeKind::UserUnion) {
    m_context->reportDiag(errNonUnionIsExpression(m_context->getSrc(), n[0].getSpan()));
    return;
  }
  const auto& unionDef = std::get<prog::sym::UnionDef>(m_context->getProg()->getTypeDef(lhsType));

  // Validate that the type is a valid type.
  const auto type = getOrInstType(m_context, m_typeSubTable, n.getType());
  if (!type) {
    m_context->reportDiag(
        errUndeclaredType(m_context->getSrc(), getName(n.getType()), n.getType().getSpan()));
    return;
  }

  // Validate that the type is part of the union.
  if (!unionDef.hasType(*type)) {
    m_context->reportDiag(errTypeNotPartOfUnion(
        m_context->getSrc(), getName(n.getType()), lhsTypeDecl.getName(), n.getType().getSpan()));
    return;
  }

  if (!n.hasId()) {
    m_expr = prog::expr::unionCheckExprNode(*m_context->getProg(), std::move(lhsExpr), *type);
    return;
  }

  // Validate that this expression is part of a checked context, meaning the const is only
  // accessed when the expression evaluates to 'true'.
  if (!m_checkedConstsAccess) {
    m_context->reportDiag(
        errUncheckedIsExpressionWithConst(m_context->getSrc(), n.getType().getSpan()));
    return;
  }

  // Declare the const.
  const auto constId = declareConst(n.getId(), *type);
  if (constId) {
    m_expr = prog::expr::unionGetExprNode(
        *m_context->getProg(), std::move(lhsExpr), *m_constBinder->getConsts(), *constId);
  }
}

auto GetExpr::visit(const parse::LitExprNode& n) -> void {
  switch (n.getVal().getKind()) {
  case lex::TokenKind::LitInt:
    m_expr = prog::expr::litIntNode(
        *m_context->getProg(), n.getVal().getPayload<lex::LitIntTokenPayload>()->getValue());
    return;
  case lex::TokenKind::LitFloat:
    m_expr = prog::expr::litFloatNode(
        *m_context->getProg(), n.getVal().getPayload<lex::LitFloatTokenPayload>()->getValue());
    return;
  case lex::TokenKind::LitBool:
    m_expr = prog::expr::litBoolNode(
        *m_context->getProg(), n.getVal().getPayload<lex::LitBoolTokenPayload>()->getValue());
    return;
  case lex::TokenKind::LitString:
    m_expr = prog::expr::litStringNode(
        *m_context->getProg(), n.getVal().getPayload<lex::LitStringTokenPayload>()->getValue());
    return;
  case lex::TokenKind::LitChar:
    m_expr = prog::expr::litCharNode(
        *m_context->getProg(), n.getVal().getPayload<lex::LitCharTokenPayload>()->getValue());
    return;
  default:
    std::stringstream oss;
    oss << n.getVal().getKind();
    m_context->reportDiag(errUnsupportedLiteral(m_context->getSrc(), oss.str(), n.getSpan()));
  }
}

auto GetExpr::visit(const parse::ParenExprNode& n) -> void {
  m_expr = getSubExpr(n[0], m_typeHint, m_checkedConstsAccess);
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
      auto condition = getSubExpr(n[i][0], &branchConsts, m_context->getProg()->getBool(), true);
      if (!condition) {
        isValid = false;
        continue;
      }
      if (applyConversion(&condition, m_context->getProg()->getBool(), n[i][0].getSpan())) {
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
    auto commonType = m_context->getProg()->findCommonType(branchTypes);
    if (!commonType) {
      m_context->reportDiag(errBranchesHaveNoCommonType(m_context->getSrc(), n.getSpan()));
      return;
    }
    m_typeHint = *commonType;
  }

  // Add a conversion for all branches that require one.
  for (auto i = 0U; i != branches.size(); ++i) {
    isValid &= applyConversion(&branches[i], m_typeHint, n[i].getSpan());
  }

  if (isValid) {
    if (!n.hasElse()) {
      if (!isExhaustive(conditions)) {
        m_context->reportDiag(errNonExhaustiveSwitchWithoutElse(m_context->getSrc(), n.getSpan()));
        return;
      }
      // When we know the switch is exhaustive then we insert a 'else' branch that is never taken.
      branches.push_back(prog::expr::failNode(branches[0]->getType()));
    }
    m_expr = prog::expr::switchExprNode(
        *m_context->getProg(), std::move(conditions), std::move(branches));
  }
}

auto GetExpr::visit(const parse::UnaryExprNode& n) -> void {
  const auto& opToken = n.getOperator();
  const auto op       = getOperator(opToken);
  if (!op) {
    m_context->reportDiag(
        errUnsupportedOperator(m_context->getSrc(), opToken.str(), opToken.getSpan()));
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
      getFunctions(funcName, std::nullopt, argTypes, n.getOperator().getSpan());
  const auto func = m_context->getProg()->lookupFunc(possibleFuncs, argTypes, 0);
  if (!func) {
    m_context->reportDiag(errUndeclaredUnaryOperator(
        m_context->getSrc(),
        getText(op.value()),
        getDisplayName(*m_context, args[0]->getType()),
        opToken.getSpan()));
    return;
  }

  m_expr = prog::expr::callExprNode(*m_context->getProg(), func.value(), std::move(args));
}

auto GetExpr::visit(const parse::ExecStmtNode & /*unused*/) -> void {
  throw std::logic_error{"GetExpr is not implemented for this node type"};
}

auto GetExpr::visit(const parse::FuncDeclStmtNode & /*unused*/) -> void {
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
  auto visitor = GetExpr{m_context, m_typeSubTable, m_constBinder, typeHint, checkedConstsAccess};
  n.accept(&visitor);
  return std::move(visitor.getValue());
}

auto GetExpr::getSubExpr(
    const parse::Node& n,
    std::vector<prog::sym::ConstId>* visibleConsts,
    prog::sym::TypeId typeHint,
    bool checkedConstsAccess) -> prog::expr::NodePtr {

  // Override the visible constants for this sub-expression.
  auto* orgVisibleConsts = m_constBinder->getVisibleConsts();
  m_constBinder->setVisibleConsts(visibleConsts);

  auto visitor = GetExpr{m_context, m_typeSubTable, m_constBinder, typeHint, checkedConstsAccess};
  n.accept(&visitor);

  m_constBinder->setVisibleConsts(orgVisibleConsts);
  return std::move(visitor.getValue());
}

auto GetExpr::applyConversion(prog::expr::NodePtr* expr, prog::sym::TypeId toType, input::Span span)
    -> bool {
  const auto fromType = (*expr)->getType();
  if (fromType == toType) {
    return true;
  }

  const auto conv = m_context->getProg()->lookupConversion(fromType, toType);
  if (!conv) {
    m_context->reportDiag(errNoConversionFound(
        m_context->getSrc(),
        getDisplayName(*m_context, fromType),
        getDisplayName(*m_context, toType),
        span));
    return false;
  }

  auto convArgs = std::vector<prog::expr::NodePtr>{};
  convArgs.push_back(std::move(*expr));
  *expr = prog::expr::callExprNode(*m_context->getProg(), *conv, std::move(convArgs));
  return true;
}

auto GetExpr::getBinLogicOpExpr(const parse::BinaryExprNode& n, BinLogicOp op)
    -> prog::expr::NodePtr {
  auto isValid = true;

  // 'And' has checked constants, meaning the constants that are declared are only accessed
  // when the expressions evaluate to 'true'.
  auto lhsCheckedConsts = op == BinLogicOp::And;
  auto lhs              = getSubExpr(n[0], m_context->getProg()->getBool(), lhsCheckedConsts);

  // Rhs might not get executed, so we only expose constants when 'checkedConstsAccess' is 'true'
  // for this expression (meaning that the constants are only observed when both lfs and rhs
  // evaluate to 'true')
  auto rhsConstantsCopy = m_checkedConstsAccess
      ? nullptr
      : std::make_unique<std::vector<prog::sym::ConstId>>(*m_constBinder->getVisibleConsts());
  auto rhs = getSubExpr(
      n[1],
      rhsConstantsCopy ? rhsConstantsCopy.get() : m_constBinder->getVisibleConsts(),
      m_context->getProg()->getBool(),
      m_checkedConstsAccess && op == BinLogicOp::And);
  if (!lhs || !rhs) {
    return nullptr;
  }

  isValid &= applyConversion(&lhs, m_context->getProg()->getBool(), n[0].getSpan());
  isValid &= applyConversion(&rhs, m_context->getProg()->getBool(), n[1].getSpan());

  if (isValid) {
    auto conditions = std::vector<prog::expr::NodePtr>{};
    conditions.push_back(std::move(lhs));

    auto branches = std::vector<prog::expr::NodePtr>{};
    switch (op) {
    case BinLogicOp::And:
      branches.push_back(std::move(rhs));
      branches.push_back(prog::expr::litBoolNode(*m_context->getProg(), false));
      break;
    case BinLogicOp::Or:
      branches.push_back(prog::expr::litBoolNode(*m_context->getProg(), true));
      branches.push_back(std::move(rhs));
    }

    return prog::expr::switchExprNode(
        *m_context->getProg(), std::move(conditions), std::move(branches));
  }
  return nullptr;
}

auto GetExpr::getConstExpr(const parse::IdExprNode& n) -> prog::expr::NodePtr {
  const auto name       = getName(n.getId());
  const auto boundConst = m_constBinder->bind(name);
  if (boundConst) {
    return prog::expr::constExprNode(*m_constBinder->getConsts(), *boundConst);
  }

  if (m_constBinder->doesExistButNotVisible(name)) {
    m_context->reportDiag(errUninitializedConst(m_context->getSrc(), name, n.getSpan()));
  } else {
    m_context->reportDiag(errUndeclaredConst(m_context->getSrc(), name, n.getSpan()));
  }
  return nullptr;
}

auto GetExpr::getDynCallExpr(const parse::CallExprNode& n) -> prog::expr::NodePtr {
  auto args = getChildExprs(n);
  if (!args) {
    assert(m_context->hasErrors());
    return nullptr;
  }

  // Check if this is a call to to a delegate type.
  if (m_context->getProg()->isDelegate(args->second[0])) {
    auto delArgs = std::vector<prog::expr::NodePtr>{};
    for (auto i = 1U; i != args->first.size(); ++i) {
      delArgs.push_back(std::move(args->first[i]));
    }
    if (!m_context->getProg()->isCallable(args->second[0], delArgs)) {
      m_context->reportDiag(errIncorrectArgsToDelegate(m_context->getSrc(), n.getSpan()));
      return nullptr;
    }
    return prog::expr::callDynExprNode(
        *m_context->getProg(), std::move(args->first[0]), std::move(delArgs));
  }

  // Check if this is a call to a overloaded call operator.
  const auto funcName      = prog::getFuncName(prog::Operator::ParenParen);
  const auto possibleFuncs = getFunctions(funcName, std::nullopt, args->second, n.getSpan());
  const auto func          = m_context->getProg()->lookupFunc(possibleFuncs, args->second, -1);
  if (!func) {
    auto argTypeNames = std::vector<std::string>{};
    for (const auto& argType : args->second) {
      argTypeNames.push_back(getDisplayName(*m_context, argType));
    }
    m_context->reportDiag(
        errUndeclaredCallOperator(m_context->getSrc(), argTypeNames, n.getSpan()));
    return nullptr;
  }

  return prog::expr::callExprNode(*m_context->getProg(), func.value(), std::move(args->first));
}

auto GetExpr::declareConst(const lex::Token& nameToken, prog::sym::TypeId type)
    -> std::optional<prog::sym::ConstId> {
  const auto constName =
      getConstName(m_context, m_typeSubTable, *m_constBinder->getConsts(), nameToken);
  if (!constName) {
    return std::nullopt;
  }
  return m_constBinder->registerLocal(*constName, type);
}

auto GetExpr::isExhaustive(const std::vector<prog::expr::NodePtr>& conditions) const -> bool {
  // Note: At the moment only union exhaustiveness check is implemented.
  auto checkUnion = CheckUnionExhaustiveness{*m_context};
  for (const auto& cond : conditions) {
    cond->accept(&checkUnion);
  }
  return checkUnion.isExhaustive();
}

auto GetExpr::getFunctionsInclConversions(
    const lex::Token& nameToken,
    const std::optional<parse::TypeParamList>& typeParams,
    const prog::sym::TypeSet& argTypes) -> std::vector<prog::sym::FuncId> {
  const auto funcName = getName(nameToken);
  auto result         = getFunctions(funcName, typeParams, argTypes, nameToken.getSpan());
  auto isValid        = true;

  // Check if this is a call to a constructor / conversion function.
  auto convType = getOrInstType(m_context, m_typeSubTable, nameToken, typeParams, argTypes);
  if (convType) {
    const auto typeName = getName(*m_context, *convType);
    const auto funcs    = m_context->getProg()->lookupFuncs(typeName);
    result.insert(result.end(), funcs.begin(), funcs.end());
  }

  return isValid ? result : std::vector<prog::sym::FuncId>{};
}

auto GetExpr::getFunctions(
    const std::string& funcName,
    const std::optional<parse::TypeParamList>& typeParams,
    const prog::sym::TypeSet& argTypes,
    input::Span span) -> std::vector<prog::sym::FuncId> {
  auto result  = std::vector<prog::sym::FuncId>{};
  auto isValid = true;

  if (typeParams) {
    // If this is a template call then instantiate the templates.
    auto typeParamSet = getTypeSet(m_context, m_typeSubTable, typeParams->getTypes());
    if (!typeParamSet) {
      assert(m_context->hasErrors());
      return {};
    }
    const auto instantiations = m_context->getFuncTemplates()->instantiate(funcName, *typeParamSet);
    for (const auto& inst : instantiations) {
      if (!inst->isSuccess()) {
        m_context->reportDiag(errInvalidFuncInstantiation(m_context->getSrc(), span));
        isValid = false;
      } else {
        result.push_back(*inst->getFunc());
      }
    }
  } else { // no type params.

    // Find all non-templated funcs.
    for (const auto& f : m_context->getProg()->lookupFuncs(funcName)) {
      result.push_back(f);
    }

    // Find templated funcs where we can infer the type params based on the argument types.
    const auto instantiations =
        m_context->getFuncTemplates()->inferParamsAndInstantiate(funcName, argTypes);
    for (const auto& inst : instantiations) {
      if (!inst->isSuccess()) {
        m_context->reportDiag(errInvalidFuncInstantiation(m_context->getSrc(), span));
        isValid = false;
      } else {
        result.push_back(*inst->getFunc());
      }
    }
  }

  return isValid ? result : std::vector<prog::sym::FuncId>{};
}

} // namespace frontend::internal
