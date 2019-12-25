#include "internal/get_expr.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/check_union_exhaustiveness.hpp"
#include "internal/utilities.hpp"
#include "lex/token_payload_lit_bool.hpp"
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
    prog::sym::ConstDeclTable* consts,
    std::vector<prog::sym::ConstId>* visibleConsts,
    prog::sym::TypeId typeHint,
    bool checkedConstsAccess) :
    m_context{context},
    m_typeSubTable{typeSubTable},
    m_consts{consts},
    m_visibleConsts{visibleConsts},
    m_typeHint{typeHint},
    m_checkedConstsAccess{checkedConstsAccess} {

  if (m_context == nullptr) {
    throw std::invalid_argument{"Context cannot be null"};
  }
  if (m_consts == nullptr) {
    throw std::invalid_argument{"Constants table cannot be null"};
  }
  if (!m_visibleConsts) {
    throw std::invalid_argument{"Visible constants vector cannot be null"};
  }
}

auto GetExpr::getValue() -> prog::expr::NodePtr& { return m_expr; }

auto GetExpr::visit(const parse::ErrorNode & /*unused*/) -> void {
  throw std::logic_error{"GetExpr is not implemented for this node type"};
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
  args.push_back(getSubExpr(n[0], m_visibleConsts, prog::sym::TypeId::inferType()));
  args.push_back(getSubExpr(n[1], m_visibleConsts, prog::sym::TypeId::inferType()));
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
    const auto& lhsTypeName = getName(m_context, args[0]->getType());
    const auto& rhsTypeName = getName(m_context, args[1]->getType());
    m_context->reportDiag(errUndeclaredBinOperator(
        m_context->getSrc(), getText(op.value()), lhsTypeName, rhsTypeName, opToken.getSpan()));
    return;
  }

  m_expr = prog::expr::callExprNode(*m_context->getProg(), func.value(), std::move(args));
}

auto GetExpr::visit(const parse::CallExprNode& n) -> void {
  auto args = getChildExprs(n);
  if (!args) {
    assert(m_context->hasErrors());
    return;
  }

  const auto possibleFuncs =
      getFunctionsInclConversions(n.getFunc(), n.getTypeParams(), args->second);
  if (m_context->hasErrors()) {
    return;
  }

  const auto func = m_context->getProg()->lookupFunc(possibleFuncs, args->second, -1);
  if (!func) {
    auto argTypeNames = std::vector<std::string>{};
    for (const auto& argType : args->second) {
      argTypeNames.push_back(getName(m_context, argType));
    }
    m_context->reportDiag(
        errUndeclaredFunc(m_context->getSrc(), getName(n.getFunc()), argTypeNames, n.getSpan()));
    return;
  }
  m_expr = prog::expr::callExprNode(*m_context->getProg(), func.value(), std::move(args->first));
}

auto GetExpr::visit(const parse::ConditionalExprNode& n) -> void {
  auto conditions = std::vector<prog::expr::NodePtr>{};
  auto branches   = std::vector<prog::expr::NodePtr>{};
  conditions.push_back(getSubExpr(n[0], m_visibleConsts, m_context->getProg()->getBool(), true));
  if (!conditions[0]) {
    return;
  }
  if (!applyConversion(&conditions[0], m_context->getProg()->getBool(), n[0].getSpan())) {
    return;
  }

  // Make a copy of the visible constants, constants should not be visible outside.
  auto ifBranchConsts = *m_visibleConsts;
  branches.push_back(getSubExpr(n[1], &ifBranchConsts, m_typeHint));

  // Make a copy of the visible constants, constants should not be visible outside.
  auto elseBranchConsts = *m_visibleConsts;
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
  auto assignExpr = getSubExpr(n[0], m_visibleConsts, prog::sym::TypeId::inferType());
  if (assignExpr == nullptr) {
    return;
  }
  const auto constId = declareConst(n.getId(), assignExpr->getType());
  if (constId) {
    m_expr = prog::expr::assignExprNode(*m_consts, constId.value(), std::move(assignExpr));
  }
}

auto GetExpr::visit(const parse::ConstExprNode& n) -> void {
  const auto& name   = getName(n.getId());
  const auto constId = m_consts->lookup(name);
  if (!constId) {
    m_context->reportDiag(errUndeclaredConst(m_context->getSrc(), name, n.getSpan()));
    return;
  }

  if (std::find(m_visibleConsts->begin(), m_visibleConsts->end(), constId.value()) ==
      m_visibleConsts->end()) {
    m_context->reportDiag(errUninitializedConst(m_context->getSrc(), name, n.getSpan()));
    return;
  }

  m_expr = prog::expr::constExprNode(*m_consts, constId.value());
}

auto GetExpr::visit(const parse::FieldExprNode& n) -> void {
  const auto& fieldName = getName(n.getId());
  auto lhsExpr          = getSubExpr(n[0], m_visibleConsts, prog::sym::TypeId::inferType());
  if (lhsExpr == nullptr) {
    return;
  }

  const auto lhsType      = lhsExpr->getType();
  const auto& lhsTypeDecl = m_context->getProg()->getTypeDecl(lhsType);
  if (lhsTypeDecl.getKind() != prog::sym::TypeKind::UserStruct) {
    m_context->reportDiag(
        errFieldNotFoundOnType(m_context->getSrc(), fieldName, lhsTypeDecl.getName(), n.getSpan()));
    return;
  }
  const auto& structDef = std::get<prog::sym::StructDef>(m_context->getProg()->getTypeDef(lhsType));
  const auto field      = structDef.getFields().lookup(fieldName);
  if (!field) {
    m_context->reportDiag(
        errFieldNotFoundOnType(m_context->getSrc(), fieldName, lhsTypeDecl.getName(), n.getSpan()));
    return;
  }

  m_expr = prog::expr::fieldExprNode(*m_context->getProg(), std::move(lhsExpr), *field);
}

auto GetExpr::visit(const parse::GroupExprNode& n) -> void {
  auto subExprs = std::vector<prog::expr::NodePtr>();
  for (auto i = 0U; i < n.getChildCount(); ++i) {
    auto last    = i == n.getChildCount() - 1;
    auto subExpr = getSubExpr(
        n[i],
        m_visibleConsts,
        last ? m_typeHint : prog::sym::TypeId::inferType(),
        m_checkedConstsAccess && last);
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
      argTypeNames.push_back(getName(m_context, argType));
    }
    m_context->reportDiag(
        errUndeclaredIndexOperator(m_context->getSrc(), argTypeNames, n.getSpan()));
    return;
  }

  m_expr = prog::expr::callExprNode(*m_context->getProg(), func.value(), std::move(args->first));
}

auto GetExpr::visit(const parse::IsExprNode& n) -> void {
  auto lhsExpr = getSubExpr(n[0], m_visibleConsts, prog::sym::TypeId::inferType());
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

  // Validate that this expression is part of a checked context, meaning the const is only accessed
  // when the expression evaluates to 'true'.
  if (!m_checkedConstsAccess) {
    m_context->reportDiag(
        errUncheckedIsExpressionWithConst(m_context->getSrc(), n.getType().getSpan()));
    return;
  }

  // Declare the const.
  const auto constId = declareConst(n.getId(), *type);
  if (constId) {
    m_expr = prog::expr::unionGetExprNode(
        *m_context->getProg(), std::move(lhsExpr), *m_consts, *constId);
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
  default:
    std::stringstream oss;
    oss << n.getVal().getKind();
    m_context->reportDiag(errUnsupportedLiteral(m_context->getSrc(), oss.str(), n.getSpan()));
  }
}

auto GetExpr::visit(const parse::ParenExprNode& n) -> void {
  m_expr = getSubExpr(n[0], m_visibleConsts, m_typeHint, m_checkedConstsAccess);
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
    auto branchConsts = *m_visibleConsts;
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
  args.push_back(getSubExpr(n[0], m_visibleConsts, prog::sym::TypeId::inferType()));
  if (!args[0] || !args[0]->getType().isConcrete()) {
    return;
  }

  const auto argTypes = prog::sym::TypeSet{{args[0]->getType()}};
  const auto funcName = prog::getFuncName(op.value());
  const auto possibleFuncs =
      getFunctions(funcName, std::nullopt, argTypes, n.getOperator().getSpan());
  const auto func = m_context->getProg()->lookupFunc(possibleFuncs, argTypes, 0);
  if (!func) {
    const auto& typeName = getName(m_context, args[0]->getType());
    m_context->reportDiag(errUndeclaredUnaryOperator(
        m_context->getSrc(), getText(op.value()), typeName, opToken.getSpan()));
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

auto GetExpr::getSubExpr(
    const parse::Node& n,
    std::vector<prog::sym::ConstId>* visibleConsts,
    prog::sym::TypeId typeHint,
    bool checkedConstsAccess) -> prog::expr::NodePtr {
  auto visitor =
      GetExpr{m_context, m_typeSubTable, m_consts, visibleConsts, typeHint, checkedConstsAccess};
  n.accept(&visitor);
  return std::move(visitor.getValue());
}

auto GetExpr::getChildExprs(const parse::Node& n)
    -> std::optional<std::pair<std::vector<prog::expr::NodePtr>, prog::sym::TypeSet>> {
  auto isValid  = true;
  auto argTypes = std::vector<prog::sym::TypeId>{};
  auto args     = std::vector<prog::expr::NodePtr>{};
  for (auto i = 0U; i < n.getChildCount(); ++i) {
    auto arg = getSubExpr(n[i], m_visibleConsts, prog::sym::TypeId::inferType());
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

auto GetExpr::applyConversion(prog::expr::NodePtr* expr, prog::sym::TypeId toType, input::Span span)
    -> bool {
  const auto fromType = (*expr)->getType();
  if (fromType == toType) {
    return true;
  }

  const auto conv = m_context->getProg()->lookupConversion(fromType, toType);
  if (!conv) {
    const auto& fromName = getName(m_context, fromType);
    const auto& toName   = getName(m_context, toType);
    m_context->reportDiag(errNoConversionFound(m_context->getSrc(), fromName, toName, span));
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
  auto lhs = getSubExpr(n[0], m_visibleConsts, m_context->getProg()->getBool(), lhsCheckedConsts);

  // Rhs might not get executed, so we only expose constants when 'checkedConstsAccess' is 'true'
  // for this expression (meaning that the constants are only observed when both lfs and rhs
  // evaluate to 'true')
  auto rhsConstantsCopy = m_checkedConstsAccess
      ? nullptr
      : std::make_unique<std::vector<prog::sym::ConstId>>(*m_visibleConsts);
  auto rhs = getSubExpr(
      n[1],
      rhsConstantsCopy ? rhsConstantsCopy.get() : m_visibleConsts,
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

auto GetExpr::declareConst(const lex::Token& nameToken, prog::sym::TypeId type)
    -> std::optional<prog::sym::ConstId> {
  const auto constName = getConstName(m_context, m_typeSubTable, *m_consts, nameToken);
  if (!constName) {
    return std::nullopt;
  }
  const auto constId = m_consts->registerLocal(*constName, type);
  m_visibleConsts->push_back(constId);
  return constId;
}

auto GetExpr::isExhaustive(const std::vector<prog::expr::NodePtr>& conditions) const -> bool {
  // Note: At the moment only union exhaustiveness check is implemented.
  auto checkUnion = CheckUnionExhaustiveness{*m_context};
  for (const auto& cond : conditions) {
    cond->accept(&checkUnion);
  }
  return checkUnion.isExhaustive();
}

auto GetExpr::isType(const std::string& name) const -> bool {
  return m_context->getProg()->lookupType(name) || m_context->getTypeTemplates()->hasType(name);
}

auto GetExpr::getFunctionsInclConversions(
    const lex::Token& nameToken,
    const std::optional<parse::TypeParamList>& typeParams,
    const prog::sym::TypeSet& argTypes) -> std::vector<prog::sym::FuncId> {
  const auto funcName = getName(nameToken);
  auto result         = getFunctions(funcName, typeParams, argTypes, nameToken.getSpan());
  auto isValid        = true;

  // Check if this is a call to a constructor / conversion function.
  std::optional<prog::sym::TypeId> convType = std::nullopt;
  if (m_typeSubTable != nullptr && m_typeSubTable->lookupType(funcName)) {
    convType = m_typeSubTable->lookupType(funcName);
  } else if (isType(funcName)) {
    // Treat this function name (including type parameters) as a type reference.
    const auto isTemplType = m_context->getTypeTemplates()->hasType(funcName);
    const auto synthesisedParseType =
        (isTemplType && typeParams) ? parse::Type{nameToken, *typeParams} : parse::Type{nameToken};
    convType = getOrInstType(m_context, m_typeSubTable, synthesisedParseType);
  }
  if (convType) {
    const auto typeName = getName(m_context, *convType);
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
  auto result  = m_context->getProg()->lookupFuncs(funcName);
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
      if (m_context->hasErrors()) {
        m_context->reportDiag(errInvalidFuncInstantiation(m_context->getSrc(), span));
        isValid = false;
      } else {
        result.push_back(*inst->getFunc());
      }
    }
  } else {
    // If no type params are given check if we can infer the type params for a function template.
    const auto instantiations =
        m_context->getFuncTemplates()->inferParamsAndInstantiate(funcName, argTypes);
    for (const auto& inst : instantiations) {
      if (m_context->hasErrors()) {
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
