#include "internal/define_user_funcs.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/get_expr.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"
#include "prog/expr/node_call.hpp"

namespace frontend::internal {

DefineUserFuncs::DefineUserFuncs(Context* context, const TypeSubstitutionTable* typeSubTable) :
    m_context{context}, m_typeSubTable{typeSubTable} {

  if (m_context == nullptr) {
    throw std::invalid_argument{"Context cannot be null"};
  }
}

auto DefineUserFuncs::define(prog::sym::FuncId id, const parse::FuncDeclStmtNode& n) -> void {
  const auto& funcDecl   = m_context->getProg()->getFuncDecl(id);
  const auto funcRetType = funcDecl.getOutput();

  auto consts = prog::sym::ConstDeclTable{};
  if (!declareInputs(n, &consts)) {
    return;
  }

  auto visibleConsts = consts.getAll();
  auto expr          = getExpr(n[0], &consts, &visibleConsts, funcRetType);

  // Report this diagnostic after processing the body so other diagnostics are also reported.
  if (funcRetType.isInfer()) {
    m_context->reportDiag(errUnableToInferFuncReturnType(
        m_context->getSrc(), getName(n.getId()), n.getId().getSpan()));
    return;
  }

  if (!expr) {
    assert(m_context->hasErrors());
    return;
  }

  if (expr->getType() == funcRetType) {
    m_context->getProg()->defineUserFunc(id, std::move(consts), std::move(expr));
    return;
  }

  const auto conv = m_context->getProg()->lookupConversion(expr->getType(), funcRetType);
  if (conv && *conv != id) {
    auto convArgs = std::vector<prog::expr::NodePtr>{};
    convArgs.push_back(std::move(expr));
    m_context->getProg()->defineUserFunc(
        id,
        std::move(consts),
        prog::expr::callExprNode(*m_context->getProg(), *conv, std::move(convArgs)));
    return;
  }

  const auto& declaredType = getName(m_context, funcDecl.getOutput());
  const auto& returnedType = getName(m_context, expr->getType());
  m_context->reportDiag(errNonMatchingFuncReturnType(
      m_context->getSrc(), getName(n.getId()), declaredType, returnedType, n[0].getSpan()));
}

auto DefineUserFuncs::declareInputs(
    const parse::FuncDeclStmtNode& n, prog::sym::ConstDeclTable* consts) -> bool {
  bool isValid = true;
  for (const auto& arg : n.getArgs()) {
    const auto constName = getConstName(m_context, m_typeSubTable, *consts, arg.getIdentifier());
    if (!constName) {
      isValid = false;
      continue;
    }

    const auto argType = getOrInstType(m_context, m_typeSubTable, arg.getType());
    if (!argType) {
      // Fail because this should have been caught during function declaration.
      throw std::logic_error{"No declaration found for function input"};
    }
    consts->registerInput(*constName, argType.value());
  }
  return isValid;
}

auto DefineUserFuncs::getExpr(
    const parse::Node& n,
    prog::sym::ConstDeclTable* consts,
    std::vector<prog::sym::ConstId>* visibleConsts,
    prog::sym::TypeId typeHint) -> prog::expr::NodePtr {

  auto getExpr = GetExpr{m_context, m_typeSubTable, consts, visibleConsts, typeHint};
  n.accept(&getExpr);
  return std::move(getExpr.getValue());
}

} // namespace frontend::internal
