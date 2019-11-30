#include "prog/expr/node_lit_string.hpp"
#include <sstream>
#include <stdexcept>

namespace prog::expr {

LitStringNode::LitStringNode(sym::TypeId type, std::string val) :
    m_type{type}, m_val{std::move(val)} {}

auto LitStringNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const LitStringNode*>(&rhs);
  return r != nullptr && m_val == r->m_val;
}

auto LitStringNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !LitStringNode::operator==(rhs);
}

auto LitStringNode::operator[](unsigned int /*unused*/) const -> const Node& {
  throw std::out_of_range{"No child at given index"};
}

auto LitStringNode::getChildCount() const -> unsigned int { return 0; }

auto LitStringNode::getType() const noexcept -> sym::TypeId { return m_type; }

auto LitStringNode::toString() const -> std::string {
  auto oss = std::ostringstream{};
  oss << '\'' << m_val << '\'';
  return oss.str();
}

auto LitStringNode::getVal() const noexcept -> const std::string& { return m_val; }

auto LitStringNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

// Factories.
auto litStringNode(const Program& program, std::string val) -> NodePtr {
  const auto type = program.lookupType("string");
  if (!type) {
    throw std::invalid_argument{"No 'string' type present in type-table"};
  }
  return std::unique_ptr<LitStringNode>{new LitStringNode{type.value(), std::move(val)}};
}

} // namespace prog::expr