#include "internal/const_remapper.hpp"
#include "internal/utilities.hpp"

namespace opt::internal {

ConstRemapper::ConstRemapper(
    const prog::Program& prog,
    const prog::sym::ConstDeclTable& consts,
    const ConstRemapTable& remapTable) :
    m_prog{prog}, m_consts{consts}, m_remapTable{remapTable}, m_modified{false} {}

auto ConstRemapper::rewrite(const prog::expr::Node& expr) -> prog::expr::NodePtr {

  switch (expr.getKind()) {
  case prog::expr::NodeKind::Assign: {
    const auto* assignExpr = expr.downcast<prog::expr::AssignExprNode>();

    m_modified     = true;
    auto newAssign = prog::expr::assignExprNode(
        m_consts, remap(assignExpr->getConst()), rewrite((*assignExpr)[0]));
    copySourceAttr(*newAssign, expr);
    return newAssign;
  }
  case prog::expr::NodeKind::Const: {
    auto* constExpr = expr.downcast<prog::expr::ConstExprNode>();

    m_modified    = true;
    auto newConst = prog::expr::constExprNode(m_consts, remap(constExpr->getId()));
    copySourceAttr(*newConst, expr);
    return newConst;
  }
  case prog::expr::NodeKind::UnionGet: {
    auto* unionGetExpr = expr.downcast<prog::expr::UnionGetExprNode>();

    m_modified       = true;
    auto newUnionGet = prog::expr::unionGetExprNode(
        m_prog, rewrite((*unionGetExpr)[0]), m_consts, remap(unionGetExpr->getConst()));
    copySourceAttr(*newUnionGet, expr);
    return newUnionGet;
  }
  default:
    return expr.clone(this);
  }
}

auto ConstRemapper::remap(prog::sym::ConstId constId) -> prog::sym::ConstId {
  auto itr = m_remapTable.find(constId);
  if (itr == m_remapTable.end()) {
    throw std::invalid_argument{"Constant-id not found in the remap table"};
  }
  return itr->second;
}

} // namespace opt::internal
