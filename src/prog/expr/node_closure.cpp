#include "prog/expr/node_closure.hpp"
#include "internal/conversion.hpp"
#include "utilities.hpp"
#include <sstream>
#include <stdexcept>

namespace prog::expr {

ClosureNode::ClosureNode(sym::TypeId type, sym::FuncId func, std::vector<NodePtr> boundArgs) :
    m_type{type}, m_func{func}, m_boundArgs{std::move(boundArgs)} {}

auto ClosureNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const ClosureNode*>(&rhs);
  return r != nullptr && m_func == r->m_func && nodesEqual(m_boundArgs, r->m_boundArgs);
}

auto ClosureNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !ClosureNode::operator==(rhs);
}

auto ClosureNode::operator[](unsigned int i) const -> const Node& {
  if (i >= m_boundArgs.size()) {
    throw std::out_of_range("No child at given index");
  }
  return *m_boundArgs[i];
}

auto ClosureNode::getChildCount() const -> unsigned int { return m_boundArgs.size(); }

auto ClosureNode::getType() const noexcept -> sym::TypeId { return m_type; }

auto ClosureNode::toString() const -> std::string {
  auto oss = std::ostringstream{};
  oss << "closure-" << m_func;
  return oss.str();
}

auto ClosureNode::getFunc() const noexcept -> sym::FuncId { return m_func; }

auto ClosureNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

// Factories.
auto closureNode(
    const Program& program, sym::TypeId type, sym::FuncId func, std::vector<NodePtr> boundArgs)
    -> NodePtr {
  if (anyNodeNull(boundArgs)) {
    throw std::invalid_argument{"Closure node cannot contain a null bound argument"};
  }
  const auto& typeDecl = program.getTypeDecl(type);
  if (typeDecl.getKind() != sym::TypeKind::UserDelegate) {
    throw std::invalid_argument{"Type is not a delegate type"};
  }
  const auto& delegateDef = std::get<sym::DelegateDef>(program.getTypeDef(type));
  const auto& funcDecl    = program.getFuncDecl(func);
  if (delegateDef.getOutput() != funcDecl.getOutput()) {
    throw std::invalid_argument{"Function signature does not match delegate definition"};
  }

  // Both the bound arguments and the delegate arguments are inputs to the function.
  auto inputTypes = std::vector<prog::sym::TypeId>{};
  for (const auto& boundArg : boundArgs) {
    inputTypes.push_back(boundArg->getType());
  }
  inputTypes.insert(inputTypes.end(), delegateDef.getInput().begin(), delegateDef.getInput().end());
  const auto inputTypeSet = prog::sym::TypeSet{std::move(inputTypes)};

  if (inputTypeSet != funcDecl.getInput()) {
    throw std::invalid_argument{
        "Bound argument types and delegate input types do not match function input"};
  }
  return std::unique_ptr<ClosureNode>{new ClosureNode{type, func, std::move(boundArgs)}};
}

} // namespace prog::expr
