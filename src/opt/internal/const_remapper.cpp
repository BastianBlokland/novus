#include "internal/const_remapper.hpp"

namespace opt::internal {

ConstRemapper::ConstRemapper(
    const prog::Program& prog,
    const prog::sym::ConstDeclTable& consts,
    const ConstRemapTable& remapTable) :
    m_prog{prog}, m_consts{consts}, m_remapTable{remapTable} {}

auto ConstRemapper::rewrite(const prog::expr::Node& expr) -> prog::expr::NodePtr {

  switch (expr.getKind()) {
  case prog::expr::NodeKind::Assign: {
    auto* assignExpr = expr.downcast<prog::expr::AssignExprNode>();
    return prog::expr::assignExprNode(
        m_consts, remap(assignExpr->getConst()), rewrite((*assignExpr)[0]));
  }
  case prog::expr::NodeKind::Const: {
    auto* constExpr = expr.downcast<prog::expr::ConstExprNode>();
    return prog::expr::constExprNode(m_consts, remap(constExpr->getId()));
  }
  case prog::expr::NodeKind::UnionGet: {
    auto* unionGetExpr = expr.downcast<prog::expr::UnionGetExprNode>();
    return prog::expr::unionGetExprNode(
        m_prog, rewrite((*unionGetExpr)[0]), m_consts, remap(unionGetExpr->getConst()));
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
