#include "prog/expr/node_union_get.hpp"
#include "utilities.hpp"
#include <sstream>
#include <stdexcept>

namespace prog::expr {

UnionGetExprNode::UnionGetExprNode(
    sym::TypeId boolType, NodePtr lhs, sym::TypeId targetType, sym::ConstId constId) :
    m_boolType{boolType}, m_lhs{std::move(lhs)}, m_targetType{targetType}, m_constId{constId} {}

auto UnionGetExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const UnionGetExprNode*>(&rhs);
  return r != nullptr && *m_lhs == *r->m_lhs && m_targetType == r->m_targetType &&
      m_constId == r->m_constId;
}

auto UnionGetExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !UnionGetExprNode::operator==(rhs);
}

auto UnionGetExprNode::operator[](unsigned int i) const -> const Node& {
  if (i == 0) {
    return *m_lhs;
  }
  throw std::out_of_range{"No child at given index"};
}

auto UnionGetExprNode::getChildCount() const -> unsigned int { return 1; }

auto UnionGetExprNode::getType() const noexcept -> sym::TypeId { return m_boolType; }

auto UnionGetExprNode::toString() const -> std::string {
  auto oss = std::ostringstream{};
  oss << "union-get-" << m_targetType << "-" << m_constId;
  return oss.str();
}

auto UnionGetExprNode::getConst() const noexcept -> sym::ConstId { return m_constId; }

auto UnionGetExprNode::getTargetType() const noexcept -> sym::TypeId { return m_targetType; }

auto UnionGetExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

// Factories.
auto unionGetExprNode(
    const Program& prog, NodePtr lhs, const sym::ConstDeclTable& constTable, sym::ConstId constId)
    -> NodePtr {
  if (!lhs) {
    throw std::invalid_argument{"Left-hand-side expression in union-get cannot be null"};
  }

  const auto& exprTypeDecl = prog.getTypeDecl(lhs->getType());
  if (exprTypeDecl.getKind() != sym::TypeKind::UserUnion) {
    throw std::invalid_argument{"Type of the left-hand-side expression needs to be a union type"};
  }
  const auto& exprTypeDef = prog.getTypeDef(lhs->getType());
  const auto& unionDef    = std::get<prog::sym::UnionDef>(exprTypeDef);

  const auto targetType = constTable[constId].getType();
  if (!unionDef.hasType(targetType)) {
    throw std::invalid_argument{
        "Type of the constant needs to be a type in the left-hand-side union"};
  }

  return std::unique_ptr<UnionGetExprNode>{
      new UnionGetExprNode{prog.getBool(), std::move(lhs), targetType, constId}};
}

} // namespace prog::expr
