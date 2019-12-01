#include "prog/expr/node_field.hpp"
#include <sstream>

namespace prog::expr {

FieldExprNode::FieldExprNode(NodePtr lhs, sym::FieldId id, sym::TypeId type) :
    m_lhs{std::move(lhs)}, m_id{id}, m_type{type} {}

auto FieldExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const FieldExprNode*>(&rhs);
  return r != nullptr && *m_lhs == *r->m_lhs && m_id == r->m_id;
}

auto FieldExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !FieldExprNode::operator==(rhs);
}

auto FieldExprNode::operator[](unsigned int i) const -> const Node& {
  if (i == 0) {
    return *m_lhs;
  }
  throw std::out_of_range{"No child at given index"};
}

auto FieldExprNode::getChildCount() const -> unsigned int { return 1; }

auto FieldExprNode::getType() const noexcept -> sym::TypeId { return m_type; }

auto FieldExprNode::toString() const -> std::string {
  auto oss = std::ostringstream{};
  oss << m_id;
  return oss.str();
}

auto FieldExprNode::getId() const noexcept -> sym::FieldId { return m_id; }

auto FieldExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

// Factories.
auto fieldExprNode(const Program& prog, NodePtr lhs, sym::FieldId id) -> NodePtr {
  const auto lhsType      = lhs->getType();
  const auto& lhsTypeDecl = prog.getTypeDecl(lhsType);
  if (lhsTypeDecl.getKind() != sym::TypeKind::UserStruct) {
    throw std::invalid_argument{"Fields are only supported on structs"};
  }
  const auto& structDef = std::get<prog::sym::StructDef>(prog.getTypeDef(lhsType));
  const auto type       = structDef.getFields()[id].getType();

  return std::unique_ptr<FieldExprNode>{new FieldExprNode{std::move(lhs), id, type}};
}

} // namespace prog::expr
