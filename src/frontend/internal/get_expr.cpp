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
#include "prog/operator.hpp"
#include <sstream>

namespace frontend::internal {

GetExpr::GetExpr(
    const Source& src,
    prog::Program* prog,
    FuncTemplateTable* funcTemplates,
    const TypeSubstitutionTable* typeSubTable,
    prog::sym::ConstDeclTable* consts,
    std::vector<prog::sym::ConstId>* visibleConsts,
    prog::sym::TypeId typeHint,
    bool checkedConstsAccess) :
    m_src{src},
    m_prog{prog},
    m_funcTemplates{funcTemplates},
    m_typeSubTable{typeSubTable},
    m_consts{consts},
    m_visibleConsts{visibleConsts},
    m_typeHint{typeHint},
    m_checkedConstsAccess{checkedConstsAccess} {

  if (m_prog == nullptr) {
    throw std::invalid_argument{"Program cannot be null"};
  }
  if (funcTemplates == nullptr) {
    throw std::invalid_argument{"Func templates table cannot be null"};
  }
  if (m_consts == nullptr) {
    throw std::invalid_argument{"Constants table cannot be null"};
  }
  if (!m_visibleConsts) {
    throw std::invalid_argument{"Visible constants vector cannot be null"};
  }
}

auto GetExpr::hasErrors() const noexcept -> bool { return !m_diags.empty(); }

auto GetExpr::getDiags() const noexcept -> const std::vector<Diag>& { return m_diags; }

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
    m_diags.push_back(errUnsupportedOperator(m_src, opToken.str(), opToken.getSpan()));
    return;
  }

  auto args = std::vector<prog::expr::NodePtr>{};
  args.push_back(getSubExpr(n[0], m_visibleConsts, prog::sym::TypeId::inferType()));
  args.push_back(getSubExpr(n[1], m_visibleConsts, prog::sym::TypeId::inferType()));
  if (!args[0] || !args[1] || !args[0]->getType().isConcrete() ||
      !args[1]->getType().isConcrete()) {
    return;
  }

  auto funcName = prog::getFuncName(op.value());
  auto func     = m_prog->lookupFunc(
      prog::getFuncName(op.value()),
      prog::sym::TypeSet{{args[0]->getType(), args[1]->getType()}},
      1);
  if (!func) {
    const auto& lhsTypeName = getName(*m_prog, args[0]->getType());
    const auto& rhsTypeName = getName(*m_prog, args[1]->getType());
    m_diags.push_back(errUndeclaredBinOperator(
        m_src, getText(op.value()), lhsTypeName, rhsTypeName, opToken.getSpan()));
    return;
  }

  m_expr = prog::expr::callExprNode(*m_prog, func.value(), std::move(args));
}

auto GetExpr::visit(const parse::CallExprNode& n) -> void {
  auto isValid         = true;
  const auto& funcName = getName(n.getFunc());

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
    return;
  }

  auto possibleFuncs = std::vector<prog::sym::FuncId>{};

  // If this is a template call then instantiate the templates.
  if (n.getTypeParams()) {
    auto typeParams =
        getTypeSet(m_src, *m_prog, m_typeSubTable, n.getTypeParams()->getTypes(), &m_diags);
    if (!typeParams) {
      assert(!m_diags.empty());
      return;
    }
    const auto instantiations = m_funcTemplates->instantiate(funcName, *typeParams);
    if (instantiations.empty()) {
      m_diags.push_back(errUndeclaredFuncTemplate(
          m_src, funcName, typeParams->getCount(), n.getFunc().getSpan()));
      return;
    }
    for (const auto& inst : instantiations) {
      if (inst->hasErrors()) {
        m_diags.push_back(errInvalidFuncInstantiation(m_src, n.getSpan()));
        const auto& instDiags = inst->getDiags();
        m_diags.insert(m_diags.end(), instDiags.begin(), instDiags.end());
        isValid = false;
      } else {
        possibleFuncs.push_back(*inst->getFunc());
      }
    }
  }
  if (!isValid) {
    return;
  }

  if (possibleFuncs.empty()) {
    possibleFuncs = m_prog->lookupFuncs(funcName);
  }
  const auto func = m_prog->lookupFunc(possibleFuncs, prog::sym::TypeSet{argTypes}, -1);
  if (!func) {
    auto argTypeNames = std::vector<std::string>{};
    for (const auto& argType : argTypes) {
      argTypeNames.push_back(getName(*m_prog, argType));
    }
    m_diags.push_back(errUndeclaredFunc(m_src, funcName, argTypeNames, n.getSpan()));
    return;
  }
  m_expr = prog::expr::callExprNode(*m_prog, func.value(), std::move(args));
}

auto GetExpr::visit(const parse::ConditionalExprNode& n) -> void {
  auto conditions = std::vector<prog::expr::NodePtr>{};
  auto branches   = std::vector<prog::expr::NodePtr>{};
  conditions.push_back(getSubExpr(n[0], m_visibleConsts, m_prog->getBool(), true));
  if (!conditions[0]) {
    return;
  }
  if (!applyConversion(&conditions[0], m_prog->getBool(), n[0].getSpan())) {
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
    auto commonType        = m_prog->findCommonType(branchTypes);
    if (!commonType) {
      m_diags.push_back(errBranchesHaveNoCommonType(m_src, n.getSpan()));
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

  m_expr = prog::expr::switchExprNode(*m_prog, std::move(conditions), std::move(branches));
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
    m_diags.push_back(errUndeclaredConst(m_src, name, n.getSpan()));
    return;
  }

  if (std::find(m_visibleConsts->begin(), m_visibleConsts->end(), constId.value()) ==
      m_visibleConsts->end()) {
    m_diags.push_back(errUninitializedConst(m_src, name, n.getSpan()));
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
  const auto& lhsTypeDecl = m_prog->getTypeDecl(lhsType);
  if (lhsTypeDecl.getKind() != prog::sym::TypeKind::UserStruct) {
    m_diags.push_back(errFieldNotFoundOnType(m_src, fieldName, lhsTypeDecl.getName(), n.getSpan()));
    return;
  }
  const auto& structDef = std::get<prog::sym::StructDef>(m_prog->getTypeDef(lhsType));
  const auto field      = structDef.getFields().lookup(fieldName);
  if (!field) {
    m_diags.push_back(errFieldNotFoundOnType(m_src, fieldName, lhsTypeDecl.getName(), n.getSpan()));
    return;
  }

  m_expr = prog::expr::fieldExprNode(*m_prog, std::move(lhsExpr), *field);
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

auto GetExpr::visit(const parse::IsExprNode& n) -> void {
  auto lhsExpr = getSubExpr(n[0], m_visibleConsts, prog::sym::TypeId::inferType());
  if (lhsExpr == nullptr) {
    return;
  }

  // Validate lhs is a union type.
  const auto lhsType      = lhsExpr->getType();
  const auto& lhsTypeDecl = m_prog->getTypeDecl(lhsType);
  if (lhsTypeDecl.getKind() != prog::sym::TypeKind::UserUnion) {
    m_diags.push_back(errNonUnionIsExpression(m_src, n[0].getSpan()));
    return;
  }
  const auto& unionDef = std::get<prog::sym::UnionDef>(m_prog->getTypeDef(lhsType));

  // Validate that the type is a valid type.
  const auto typeName = getName(n.getType());
  const auto type     = getType(*m_prog, m_typeSubTable, typeName);
  if (!type) {
    m_diags.push_back(errUndeclaredType(m_src, typeName, n.getType().getSpan()));
    return;
  }

  // Validate that the type is part of the union.
  if (!unionDef.hasType(*type)) {
    m_diags.push_back(
        errTypeNotPartOfUnion(m_src, typeName, lhsTypeDecl.getName(), n.getType().getSpan()));
    return;
  }

  if (!n.hasId()) {
    m_expr = prog::expr::unionCheckExprNode(*m_prog, std::move(lhsExpr), *type);
    return;
  }

  // Validate that this expression is part of a checked context, meaning the const is only accessed
  // when the expression evaluates to 'true'.
  if (!m_checkedConstsAccess) {
    m_diags.push_back(errUncheckedIsExpressionWithConst(m_src, n.getType().getSpan()));
    return;
  }

  // Declare the const.
  const auto constId = declareConst(n.getId(), *type);
  if (constId) {
    m_expr = prog::expr::unionGetExprNode(*m_prog, std::move(lhsExpr), *m_consts, *constId);
  }
}

auto GetExpr::visit(const parse::LitExprNode& n) -> void {
  switch (n.getVal().getKind()) {
  case lex::TokenKind::LitInt:
    m_expr = prog::expr::litIntNode(
        *m_prog, n.getVal().getPayload<lex::LitIntTokenPayload>()->getValue());
    return;
  case lex::TokenKind::LitFloat:
    m_expr = prog::expr::litFloatNode(
        *m_prog, n.getVal().getPayload<lex::LitFloatTokenPayload>()->getValue());
    return;
  case lex::TokenKind::LitBool:
    m_expr = prog::expr::litBoolNode(
        *m_prog, n.getVal().getPayload<lex::LitBoolTokenPayload>()->getValue());
    return;
  case lex::TokenKind::LitString:
    m_expr = prog::expr::litStringNode(
        *m_prog, n.getVal().getPayload<lex::LitStringTokenPayload>()->getValue());
    return;
  default:
    std::stringstream oss;
    oss << n.getVal().getKind();
    m_diags.push_back(errUnsupportedLiteral(m_src, oss.str(), n.getSpan()));
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
      auto condition = getSubExpr(n[i][0], &branchConsts, m_prog->getBool(), true);
      if (!condition) {
        isValid = false;
        continue;
      }
      if (applyConversion(&condition, m_prog->getBool(), n[i][0].getSpan())) {
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
    auto commonType = m_prog->findCommonType(branchTypes);
    if (!commonType) {
      m_diags.push_back(errBranchesHaveNoCommonType(m_src, n.getSpan()));
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
        m_diags.push_back(errNonExhaustiveSwitchWithoutElse(m_src, n.getSpan()));
        return;
      }
      // When we know the switch is exhaustive then we insert a 'else' branch that is never taken.
      branches.push_back(prog::expr::failNode(branches[0]->getType()));
    }
    m_expr = prog::expr::switchExprNode(*m_prog, std::move(conditions), std::move(branches));
  }
}

auto GetExpr::visit(const parse::UnaryExprNode& n) -> void {
  const auto& opToken = n.getOperator();
  const auto op       = getOperator(opToken);
  if (!op) {
    m_diags.push_back(errUnsupportedOperator(m_src, opToken.str(), opToken.getSpan()));
    return;
  }

  auto args = std::vector<prog::expr::NodePtr>{};
  args.push_back(getSubExpr(n[0], m_visibleConsts, prog::sym::TypeId::inferType()));
  if (!args[0] || !args[0]->getType().isConcrete()) {
    return;
  }

  auto funcName = prog::getFuncName(op.value());
  auto func     = m_prog->lookupFunc(
      prog::getFuncName(op.value()), prog::sym::TypeSet{{args[0]->getType()}}, 0);
  if (!func) {
    const auto& typeName = getName(*m_prog, args[0]->getType());
    m_diags.push_back(
        errUndeclaredUnaryOperator(m_src, getText(op.value()), typeName, opToken.getSpan()));
    return;
  }

  m_expr = prog::expr::callExprNode(*m_prog, func.value(), std::move(args));
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
  auto visitor = GetExpr{m_src,
                         m_prog,
                         m_funcTemplates,
                         m_typeSubTable,
                         m_consts,
                         visibleConsts,
                         typeHint,
                         checkedConstsAccess};
  n.accept(&visitor);
  m_diags.insert(m_diags.end(), visitor.getDiags().begin(), visitor.getDiags().end());
  return std::move(visitor.getValue());
}

auto GetExpr::applyConversion(prog::expr::NodePtr* expr, prog::sym::TypeId toType, input::Span span)
    -> bool {
  const auto fromType = (*expr)->getType();
  if (fromType == toType) {
    return true;
  }

  const auto conv = m_prog->lookupConversion(fromType, toType);
  if (!conv) {
    const auto& fromName = getName(*m_prog, fromType);
    const auto& toName   = getName(*m_prog, toType);
    m_diags.push_back(errNoConversionFound(m_src, fromName, toName, span));
    return false;
  }

  auto convArgs = std::vector<prog::expr::NodePtr>{};
  convArgs.push_back(std::move(*expr));
  *expr = prog::expr::callExprNode(*m_prog, *conv, std::move(convArgs));
  return true;
}

auto GetExpr::getBinLogicOpExpr(const parse::BinaryExprNode& n, BinLogicOp op)
    -> prog::expr::NodePtr {
  auto isValid = true;

  // 'And' has checked consts for the lhs, meaning the constants the lhs declares are only accessed
  // when the lhs expression evaluates to 'true'.
  auto checkedConsts = op == BinLogicOp::And;
  auto lhs           = getSubExpr(n[0], m_visibleConsts, m_prog->getBool(), checkedConsts);

  // Because the rhs might not get executed the constants it declares are not visible outside.
  auto rhsVisibleConsts = *m_visibleConsts;
  auto rhs              = getSubExpr(n[1], &rhsVisibleConsts, m_prog->getBool());
  if (!lhs || !rhs) {
    return nullptr;
  }

  isValid &= applyConversion(&lhs, m_prog->getBool(), n[0].getSpan());
  isValid &= applyConversion(&rhs, m_prog->getBool(), n[1].getSpan());

  if (isValid) {
    auto conditions = std::vector<prog::expr::NodePtr>{};
    conditions.push_back(std::move(lhs));

    auto branches = std::vector<prog::expr::NodePtr>{};
    switch (op) {
    case BinLogicOp::And:
      branches.push_back(std::move(rhs));
      branches.push_back(prog::expr::litBoolNode(*m_prog, false));
      break;
    case BinLogicOp::Or:
      branches.push_back(prog::expr::litBoolNode(*m_prog, true));
      branches.push_back(std::move(rhs));
    }

    return prog::expr::switchExprNode(*m_prog, std::move(conditions), std::move(branches));
  }
  return nullptr;
}

auto GetExpr::declareConst(const lex::Token& nameToken, prog::sym::TypeId type)
    -> std::optional<prog::sym::ConstId> {
  const auto constName =
      getConstName(m_src, *m_prog, m_typeSubTable, *m_consts, nameToken, &m_diags);
  if (!constName) {
    return std::nullopt;
  }
  const auto constId = m_consts->registerLocal(*constName, type);
  m_visibleConsts->push_back(constId);
  return constId;
}

auto GetExpr::isExhaustive(const std::vector<prog::expr::NodePtr>& conditions) const -> bool {
  // Note: At the moment only union exhaustiveness check is implemented.
  auto checkUnion = CheckUnionExhaustiveness{*m_prog};
  for (const auto& cond : conditions) {
    cond->accept(&checkUnion);
  }
  return checkUnion.isExhaustive();
}

} // namespace frontend::internal
