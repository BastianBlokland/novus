#include "prog/expr/node_lit_char.hpp"
#include <sstream>
#include <stdexcept>

namespace prog::expr {

LitCharNode::LitCharNode(sym::TypeId type, char val) : m_type{type}, m_val{val} {}

auto LitCharNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const LitCharNode*>(&rhs);
  return r != nullptr && m_val == r->m_val;
}

auto LitCharNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !LitCharNode::operator==(rhs);
}

auto LitCharNode::operator[](unsigned int /*unused*/) const -> const Node& {
  throw std::out_of_range{"No child at given index"};
}

auto LitCharNode::getChildCount() const -> unsigned int { return 0; }

auto LitCharNode::getType() const noexcept -> sym::TypeId { return m_type; }

auto LitCharNode::toString() const -> std::string {
  auto oss = std::ostringstream{};
  oss << '\'' << m_val << '\'';
  return oss.str();
}

auto LitCharNode::getVal() const noexcept -> char { return m_val; }

auto LitCharNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

// Factories.
auto litCharNode(const Program& program, char val) -> NodePtr {
  return std::unique_ptr<LitCharNode>{new LitCharNode{program.getChar(), val}};
}

} // namespace prog::expr
