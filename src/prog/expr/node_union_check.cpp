#include "prog/expr/node_union_check.hpp"
#include "utilities.hpp"
#include <sstream>
#include <stdexcept>

namespace prog::expr {

UnionCheckExprNode::UnionCheckExprNode(sym::TypeId boolType, NodePtr lhs, sym::TypeId targetType) :
    m_boolType{boolType}, m_lhs{std::move(lhs)}, m_targetType{targetType} {}

auto UnionCheckExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const UnionCheckExprNode*>(&rhs);
  return r != nullptr && *m_lhs == *r->m_lhs && m_targetType == r->m_targetType;
}

auto UnionCheckExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !UnionCheckExprNode::operator==(rhs);
}

auto UnionCheckExprNode::operator[](unsigned int i) const -> const Node& {
  if (i == 0) {
    return *m_lhs;
  }
  throw std::out_of_range{"No child at given index"};
}

auto UnionCheckExprNode::getChildCount() const -> unsigned int { return 1; }

auto UnionCheckExprNode::getType() const noexcept -> sym::TypeId { return m_boolType; }

auto UnionCheckExprNode::toString() const -> std::string {
  auto oss = std::ostringstream{};
  oss << "union-check-" << m_targetType;
  return oss.str();
}

auto UnionCheckExprNode::getTargetType() const noexcept -> sym::TypeId { return m_targetType; }

auto UnionCheckExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

// Factories.
auto unionCheckExprNode(const Program& prog, NodePtr lhs, sym::TypeId targetType) -> NodePtr {
  if (!lhs) {
    throw std::invalid_argument{"Left-hand-side expression in union-check cannot be null"};
  }

  const auto& exprTypeDecl = prog.getTypeDecl(lhs->getType());
  if (exprTypeDecl.getKind() != sym::TypeKind::Union) {
    throw std::invalid_argument{"Type of the left-hand-side expression needs to be a union type"};
  }
  const auto& exprTypeDef = prog.getTypeDef(lhs->getType());
  const auto& unionDef    = std::get<prog::sym::UnionDef>(exprTypeDef);

  if (!unionDef.hasType(targetType)) {
    throw std::invalid_argument{
        "Type of the constant needs to be a type in the left-hand-side union"};
  }

  return std::unique_ptr<UnionCheckExprNode>{
      new UnionCheckExprNode{prog.getBool(), std::move(lhs), targetType}};
}

} // namespace prog::expr
