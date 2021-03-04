#include "prog/expr/node_lit_enum.hpp"
#include <stdexcept>

namespace prog::expr {

LitEnumNode::LitEnumNode(sym::TypeId type, std::string entryName, int32_t val) :
    Node{LitEnumNode::getKind()}, m_type{type}, m_name{std::move(entryName)}, m_val{val} {}

auto LitEnumNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const LitEnumNode*>(&rhs);
  return r != nullptr && m_type == r->m_type && m_val == r->m_val;
}

auto LitEnumNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !LitEnumNode::operator==(rhs);
}

auto LitEnumNode::operator[](unsigned int /*unused*/) const -> const Node& {
  throw std::out_of_range{"No child at given index"};
}

auto LitEnumNode::getChildCount() const -> unsigned int { return 0; }

auto LitEnumNode::getType() const noexcept -> sym::TypeId { return m_type; }

auto LitEnumNode::toString() const -> std::string { return m_name; }

auto LitEnumNode::clone(Rewriter* /*rewriter*/) const -> std::unique_ptr<Node> {
  auto* newExpr = new LitEnumNode{m_type, m_name, m_val};
  newExpr->setSourceId(getSourceId());
  return std::unique_ptr<LitEnumNode>{newExpr};
}

auto LitEnumNode::getName() const noexcept -> const std::string& { return m_name; }

auto LitEnumNode::getVal() const noexcept -> int32_t { return m_val; }

auto LitEnumNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

// Factories.
auto litEnumNode(const Program& prog, sym::TypeId enumType, std::string name) -> NodePtr {
  const auto& enumTypeDecl = prog.getTypeDecl(enumType);
  if (enumTypeDecl.getKind() != sym::TypeKind::Enum) {
    throw std::invalid_argument{"Given type is not an enum"};
  }
  const auto& enumDef = std::get<prog::sym::EnumDef>(prog.getTypeDef(enumType));
  const auto val      = enumDef.getValue(name);
  if (!val) {
    throw std::invalid_argument{"Given name is not part of an enum"};
  }
  return std::unique_ptr<LitEnumNode>{new LitEnumNode{enumType, std::move(name), *val}};
}

auto litEnumNode(const Program& prog, sym::TypeId enumType, int32_t val) -> NodePtr {
  const auto& enumTypeDecl = prog.getTypeDecl(enumType);
  if (enumTypeDecl.getKind() != sym::TypeKind::Enum) {
    throw std::invalid_argument{"Given type is not an enum"};
  }
  return std::unique_ptr<LitEnumNode>{new LitEnumNode{enumType, std::to_string(val), val}};
}

} // namespace prog::expr
