#include "internal/get_expr.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/utilities.hpp"
#include "lex/token_payload_lit_bool.hpp"
#include "lex/token_payload_lit_int.hpp"
#include "parse/nodes.hpp"
#include "prog/expr/nodes.hpp"
#include "prog/operator.hpp"
#include <sstream>

namespace frontend::internal {

GetExpr::GetExpr(
    const Source& src,
    prog::Program* prog,
    prog::sym::ConstDeclTable* consts,
    std::vector<prog::sym::ConstId>* visibleConsts) :
    m_src{src}, m_prog{prog}, m_consts{consts}, m_visibleConsts{visibleConsts} {
  if (m_prog == nullptr) {
    throw std::invalid_argument{"Program cannot be null"};
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
  args.push_back(getSubExpr(n[0], m_visibleConsts));
  args.push_back(getSubExpr(n[1], m_visibleConsts));
  if (!args[0] || !args[1] || !args[0]->getType().isConcrete() ||
      !args[1]->getType().isConcrete()) {
    return;
  }

  auto funcName = prog::getFuncName(op.value());
  auto func     = m_prog->lookupFunc(
      prog::getFuncName(op.value()), prog::sym::Input{{args[0]->getType(), args[1]->getType()}});
  if (!func) {
    const auto& lhsTypeName = m_prog->getTypeDecl(args[0]->getType()).getName();
    const auto& rhsTypeName = m_prog->getTypeDecl(args[1]->getType()).getName();
    m_diags.push_back(errUndeclaredBinOperator(
        m_src, getText(op.value()), lhsTypeName, rhsTypeName, opToken.getSpan()));
    return;
  }

  m_expr = prog::expr::callExprNode(*m_prog, func.value(), std::move(args));
}

auto GetExpr::visit(const parse::CallExprNode& n) -> void {
  auto isValid  = true;
  auto argTypes = std::vector<prog::sym::TypeId>{};
  auto args     = std::vector<prog::expr::NodePtr>{};
  for (auto i = 0U; i < n.getChildCount(); ++i) {
    auto arg = getSubExpr(n[i], m_visibleConsts);
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

  const auto& funcName = getName(n.getFunc());
  const auto func      = m_prog->lookupFunc(funcName, prog::sym::Input{argTypes});
  if (!func) {
    if (m_prog->lookupFuncs(funcName).empty()) {
      m_diags.push_back(errUndeclaredFunc(m_src, funcName, n.getFunc().getSpan()));
    } else {
      auto argTypeNames = std::vector<std::string>{};
      for (const auto& argType : argTypes) {
        argTypeNames.push_back(m_prog->getTypeDecl(argType).getName());
      }
      m_diags.push_back(errUndeclaredFuncOverload(m_src, funcName, argTypeNames, n.getSpan()));
    }
    return;
  }
  m_expr = prog::expr::callExprNode(*m_prog, func.value(), std::move(args));
}

auto GetExpr::visit(const parse::ConditionalExprNode& n) -> void {
  auto conditions = std::vector<prog::expr::NodePtr>();
  auto branches   = std::vector<prog::expr::NodePtr>();
  conditions.push_back(getSubExpr(n[0], m_visibleConsts));
  if (!conditions[0]) {
    return;
  }
  if (!isBoolType(conditions[0]->getType())) {
    const auto& typeName = m_prog->getTypeDecl(conditions[0]->getType()).getName();
    m_diags.push_back(errNonBoolConditionExpression(m_src, typeName, n[0].getSpan()));
    return;
  }

  // Make a copy of the visible constants, constants should not be visible outside.
  auto ifBranchConsts = *m_visibleConsts;
  branches.push_back(getSubExpr(n[1], &ifBranchConsts));

  // Make a copy of the visible constants, constants should not be visible outside.
  auto elseBranchConsts = *m_visibleConsts;
  branches.push_back(getSubExpr(n[2], &elseBranchConsts));

  if (!branches[0] || !branches[1]) {
    return;
  }
  if (branches[0]->getType() != branches[1]->getType()) {
    const auto& typeName       = m_prog->getTypeDecl(branches[0]->getType()).getName();
    const auto& branchTypeName = m_prog->getTypeDecl(branches[1]->getType()).getName();
    m_diags.push_back(errMismatchedBranchTypes(m_src, typeName, branchTypeName, n[2].getSpan()));
    return;
  }

  m_expr = prog::expr::switchExprNode(*m_prog, std::move(conditions), std::move(branches));
}

auto GetExpr::visit(const parse::ConstDeclExprNode& n) -> void {
  auto assignExpr = getSubExpr(n[0], m_visibleConsts);
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

auto GetExpr::visit(const parse::GroupExprNode& n) -> void {
  auto subExprs = std::vector<prog::expr::NodePtr>();
  for (auto i = 0U; i < n.getChildCount(); ++i) {
    auto subExpr = getSubExpr(n[i], m_visibleConsts);
    if (subExpr) {
      subExprs.push_back(std::move(subExpr));
    }
  }
  if (subExprs.size() > 1) {
    m_expr = prog::expr::groupExprNode(std::move(subExprs));
  }
}

auto GetExpr::visit(const parse::LitExprNode& n) -> void {
  switch (n.getVal().getKind()) {
  case lex::TokenKind::LitInt:
    m_expr = prog::expr::litIntNode(
        *m_prog, n.getVal().getPayload<lex::LitIntTokenPayload>()->getValue());
    return;
  case lex::TokenKind::LitBool:
    m_expr = prog::expr::litBoolNode(
        *m_prog, n.getVal().getPayload<lex::LitBoolTokenPayload>()->getValue());
    return;
  default:
    std::stringstream oss;
    oss << n.getVal().getKind();
    m_diags.push_back(errUnsupportedLiteral(m_src, oss.str(), n.getSpan()));
  }
}

auto GetExpr::visit(const parse::ParenExprNode& n) -> void {
  m_expr = getSubExpr(n[0], m_visibleConsts);
}

auto GetExpr::visit(const parse::SwitchExprElseNode & /*unused*/) -> void {
  throw std::logic_error{"GetExpr is not implemented for this node type"};
}

auto GetExpr::visit(const parse::SwitchExprIfNode & /*unused*/) -> void {
  throw std::logic_error{"GetExpr is not implemented for this node type"};
}

auto GetExpr::visit(const parse::SwitchExprNode& n) -> void {
  auto isValid                          = true;
  auto conditions                       = std::vector<prog::expr::NodePtr>();
  auto branches                         = std::vector<prog::expr::NodePtr>();
  std::optional<prog::sym::TypeId> type = std::nullopt;

  for (auto i = 0U; i < n.getChildCount(); ++i) {
    const auto isElseClause = i == n.getChildCount() - 1;

    // Keep a separate set of visible constants, because consts declared in 1 branch should not
    // be allowed to be accessed from another branch or after the switch.
    auto branchConsts = *m_visibleConsts;
    if (!isElseClause) {
      auto condition = getSubExpr(n[i][0], &branchConsts);
      if (!condition) {
        isValid = false;
        continue;
      }
      if (!isBoolType(condition->getType())) {
        const auto& typeName = m_prog->getTypeDecl(condition->getType()).getName();
        m_diags.push_back(errNonBoolConditionExpression(m_src, typeName, n[i][0].getSpan()));
        isValid = false;
        continue;
      }
      conditions.push_back(std::move(condition));
    }

    auto branch = getSubExpr(n[i][isElseClause ? 0 : 1], &branchConsts);
    if (!branch) {
      isValid = false;
      continue;
    }
    if (!type) {
      type = branch->getType();
    } else if (branch->getType() != type) {
      const auto& typeName       = m_prog->getTypeDecl(type.value()).getName();
      const auto& branchTypeName = m_prog->getTypeDecl(branch->getType()).getName();
      m_diags.push_back(errMismatchedBranchTypes(
          m_src, typeName, branchTypeName, n[i][isElseClause ? 0 : 1].getSpan()));
      isValid = false;
      continue;
    }
    branches.push_back(std::move(branch));
  }

  if (isValid) {
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
  args.push_back(getSubExpr(n[0], m_visibleConsts));
  if (!args[0] || !args[0]->getType().isConcrete()) {
    return;
  }

  auto funcName = prog::getFuncName(op.value());
  auto func =
      m_prog->lookupFunc(prog::getFuncName(op.value()), prog::sym::Input{{args[0]->getType()}});
  if (!func) {
    const auto& typeName = m_prog->getTypeDecl(args[0]->getType()).getName();
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

auto GetExpr::getSubExpr(const parse::Node& n, std::vector<prog::sym::ConstId>* visibleConsts)
    -> prog::expr::NodePtr {
  auto visitor = GetExpr{m_src, m_prog, m_consts, visibleConsts};
  n.accept(&visitor);
  m_diags.insert(m_diags.end(), visitor.getDiags().begin(), visitor.getDiags().end());
  return std::move(visitor.getValue());
}

auto GetExpr::getBinLogicOpExpr(const parse::BinaryExprNode& n, BinLogicOp op)
    -> prog::expr::NodePtr {
  auto isValid = true;
  auto lhs     = getSubExpr(n[0], m_visibleConsts);

  // Because the rhs might not get executed the constants it declares are not visible outside.
  auto rhsVisibleConsts = *m_visibleConsts;
  auto rhs              = getSubExpr(n[1], &rhsVisibleConsts);
  if (!lhs || !rhs) {
    return nullptr;
  }

  if (!isBoolType(lhs->getType())) {
    const auto& typeName = m_prog->getTypeDecl(lhs->getType()).getName();
    m_diags.push_back(errNonBoolExpressionInLogicOp(m_src, typeName, n[0].getSpan()));
    isValid = false;
  }
  if (!isBoolType(rhs->getType())) {
    const auto& typeName = m_prog->getTypeDecl(rhs->getType()).getName();
    m_diags.push_back(errNonBoolExpressionInLogicOp(m_src, typeName, n[1].getSpan()));
    isValid = false;
  }

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
  const auto name = getName(nameToken);
  if (m_prog->lookupType(name)) {
    m_diags.push_back(errConstNameConflictsWithType(m_src, name, nameToken.getSpan()));
    return std::nullopt;
  }
  if (!m_prog->lookupFuncs(name).empty()) {
    m_diags.push_back(errConstNameConflictsWithFunction(m_src, name, nameToken.getSpan()));
    return std::nullopt;
  }
  if (!m_prog->lookupActions(name).empty()) {
    m_diags.push_back(errConstNameConflictsWithAction(m_src, name, nameToken.getSpan()));
    return std::nullopt;
  }
  if (m_consts->lookup(name)) {
    m_diags.push_back(errConstNameConflictsWithConst(m_src, name, nameToken.getSpan()));
    return std::nullopt;
  }
  const auto constId = m_consts->registerLocal(name, type);
  m_visibleConsts->push_back(constId);
  return constId;
}

auto GetExpr::isBoolType(prog::sym::TypeId type) -> bool {
  auto boolType = m_prog->lookupType("bool");
  if (!boolType) {
    throw std::logic_error{"No 'bool' type present in type-table"};
  }
  return type == boolType;
}

} // namespace frontend::internal
