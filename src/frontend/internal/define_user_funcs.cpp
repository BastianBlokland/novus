#include "internal/define_user_funcs.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/check_inf_recursion.hpp"
#include "internal/const_binder.hpp"
#include "internal/get_expr.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"
#include "prog/expr/node_call.hpp"
#include <cassert>

namespace frontend::internal {

DefineUserFuncs::DefineUserFuncs(Context* context, const TypeSubstitutionTable* typeSubTable) :
    m_context{context}, m_typeSubTable{typeSubTable} {

  if (m_context == nullptr) {
    throw std::invalid_argument{"Context cannot be null"};
  }
}

template <typename FuncParseNode>
auto DefineUserFuncs::define(
    prog::sym::FuncId id, std::string funcDisplayName, const FuncParseNode& n) -> bool {
  const auto& funcDecl   = m_context->getProg()->getFuncDecl(id);
  const auto funcRetType = funcDecl.getOutput();

  auto consts = prog::sym::ConstDeclTable{};
  if (!declareFuncInput(m_context, m_typeSubTable, n, &consts)) {
    return false;
  }

  auto allowActionCalls = funcDecl.isAction();
  auto visibleConsts    = consts.getAll();
  auto expr             = getExpr(n[0], &consts, &visibleConsts, funcRetType, allowActionCalls);

  // Report this diagnostic after processing the body so other errors have priority over this.
  if (funcRetType.isInfer()) {
    if (!m_context->hasErrors()) {
      m_context->reportDiag(
          errUnableToInferFuncReturnType(m_context->getSrc(), funcDisplayName, n.getSpan()));
    }
    return false;
  }

  // Bail out if we failed to get an expression for the body.
  if (!expr) {
    assert(m_context->hasErrors());
    return false;
  }

  // Check if a pure functions contains infinite recursion.
  if (!funcDecl.isAction()) {
    auto checkInfRec = CheckInfRecursion{*m_context, id};
    expr->accept(&checkInfRec);
    if (checkInfRec.isInfRecursion()) {
      m_context->reportDiag(errPureFuncInfRecursion(m_context->getSrc(), n[0].getSpan()));
      return false;
    }
  }

  if (expr->getType() == funcRetType) {
    m_context->getProg()->defineFunc(id, std::move(consts), std::move(expr));
    return true;
  }

  const auto conv = m_context->getProg()->lookupConversion(expr->getType(), funcRetType);
  if (conv && *conv != id) {
    auto convArgs = std::vector<prog::expr::NodePtr>{};
    convArgs.push_back(std::move(expr));
    m_context->getProg()->defineFunc(
        id,
        std::move(consts),
        prog::expr::callExprNode(*m_context->getProg(), *conv, std::move(convArgs)));
    return true;
  }

  m_context->reportDiag(errNonMatchingFuncReturnType(
      m_context->getSrc(),
      funcDisplayName,
      getDisplayName(*m_context, funcRetType),
      getDisplayName(*m_context, expr->getType()),
      n[0].getSpan()));
  return false;
}

auto DefineUserFuncs::getExpr(
    const parse::Node& n,
    prog::sym::ConstDeclTable* consts,
    std::vector<prog::sym::ConstId>* visibleConsts,
    prog::sym::TypeId typeHint,
    bool allowActionCalls) -> prog::expr::NodePtr {

  auto constBinder = ConstBinder{consts, visibleConsts, nullptr};
  auto getExpr =
      GetExpr{m_context,
              m_typeSubTable,
              &constBinder,
              typeHint,
              allowActionCalls ? GetExpr::Flags::AllowActionCalls : GetExpr::Flags::None};
  n.accept(&getExpr);
  return std::move(getExpr.getValue());
}

// Explicit instantiations.
template bool DefineUserFuncs::define(
    prog::sym::FuncId id, std::string funcDisplayName, const parse::FuncDeclStmtNode& n);
template bool DefineUserFuncs::define(
    prog::sym::FuncId id, std::string funcDisplayName, const parse::AnonFuncExprNode& n);

} // namespace frontend::internal
