#include "prog/expr/node_lit_func.hpp"
#include "prog/sym/delegate_def.hpp"
#include <sstream>
#include <stdexcept>

namespace prog::expr {

LitFuncNode::LitFuncNode(sym::TypeId type, sym::FuncId func) : m_type{type}, m_func{func} {}

auto LitFuncNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const LitFuncNode*>(&rhs);
  return r != nullptr && m_func == r->m_func;
}

auto LitFuncNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !LitFuncNode::operator==(rhs);
}

auto LitFuncNode::operator[](unsigned int /*unused*/) const -> const Node& {
  throw std::out_of_range{"No child at given index"};
}

auto LitFuncNode::getChildCount() const -> unsigned int { return 0; }

auto LitFuncNode::getType() const noexcept -> sym::TypeId { return m_type; }

auto LitFuncNode::toString() const -> std::string {
  auto oss = std::ostringstream{};
  oss << m_func;
  return oss.str();
}

auto LitFuncNode::getFunc() const noexcept -> sym::FuncId { return m_func; }

auto LitFuncNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

// Factories.
auto litFuncNode(const Program& program, sym::TypeId type, sym::FuncId func) -> NodePtr {
  const auto& typeDecl = program.getTypeDecl(type);
  if (typeDecl.getKind() != sym::TypeKind::UserDelegate) {
    throw std::invalid_argument{"Type is not a delegate type"};
  }
  const auto& delegateDef = std::get<sym::DelegateDef>(program.getTypeDef(type));
  const auto& funcDecl    = program.getFuncDecl(func);
  if (delegateDef.getInput() != funcDecl.getInput() ||
      delegateDef.getOutput() != funcDecl.getOutput()) {
    throw std::invalid_argument{"Function signature does not match delegate definition"};
  }
  return std::unique_ptr<LitFuncNode>{new LitFuncNode{type, func}};
}

} // namespace prog::expr
