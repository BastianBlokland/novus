#include "internal/check_union_exhaustiveness.hpp"
#include "prog/expr/nodes.hpp"

namespace frontend::internal {

CheckUnionExhaustiveness::CheckUnionExhaustiveness(const Context& context) : m_context{context} {}

auto CheckUnionExhaustiveness::isExhaustive() const -> bool {
  if (!m_unionType) {
    return false;
  }
  const auto& unionDef =
      std::get<prog::sym::UnionDef>(m_context.getProg()->getTypeDef(*m_unionType));

  // Validate that all union-types have been checked.
  for (const auto& type : unionDef.getTypes()) {
    if (std::find(m_checkedTypes.begin(), m_checkedTypes.end(), type) == m_checkedTypes.end()) {
      return false;
    }
  }
  return true;
}

auto CheckUnionExhaustiveness::visit(const prog::expr::AssignExprNode & /*unused*/) -> void {}

auto CheckUnionExhaustiveness::visit(const prog::expr::SwitchExprNode & /*unused*/) -> void {}

auto CheckUnionExhaustiveness::visit(const prog::expr::CallExprNode & /*unused*/) -> void {}

auto CheckUnionExhaustiveness::visit(const prog::expr::CallDynExprNode & /*unused*/) -> void {}

auto CheckUnionExhaustiveness::visit(const prog::expr::ClosureNode & /*unused*/) -> void {}

auto CheckUnionExhaustiveness::visit(const prog::expr::ConstExprNode & /*unused*/) -> void {}

auto CheckUnionExhaustiveness::visit(const prog::expr::FieldExprNode & /*unused*/) -> void {}

auto CheckUnionExhaustiveness::visit(const prog::expr::GroupExprNode & /*unused*/) -> void {}

auto CheckUnionExhaustiveness::visit(const prog::expr::UnionCheckExprNode& n) -> void {
  const auto unionType = n[0].getType();
  if (m_unionType && m_unionType != unionType) {
    return;
  }
  m_unionType = unionType;
  m_checkedTypes.push_back(n.getTargetType());
}

auto CheckUnionExhaustiveness::visit(const prog::expr::UnionGetExprNode& n) -> void {
  const auto unionType = n[0].getType();
  if (m_unionType && m_unionType != unionType) {
    return;
  }
  m_unionType = unionType;
  m_checkedTypes.push_back(n.getTargetType());
}

auto CheckUnionExhaustiveness::visit(const prog::expr::FailNode & /*unused*/) -> void {}

auto CheckUnionExhaustiveness::visit(const prog::expr::LitBoolNode & /*unused*/) -> void {}

auto CheckUnionExhaustiveness::visit(const prog::expr::LitFloatNode & /*unused*/) -> void {}

auto CheckUnionExhaustiveness::visit(const prog::expr::LitFuncNode & /*unused*/) -> void {}

auto CheckUnionExhaustiveness::visit(const prog::expr::LitIntNode & /*unused*/) -> void {}

auto CheckUnionExhaustiveness::visit(const prog::expr::LitStringNode & /*unused*/) -> void {}

} // namespace frontend::internal
