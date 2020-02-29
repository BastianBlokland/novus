#include "prog/sym/func_def_table.hpp"
#include <stdexcept>

namespace prog::sym {

auto FuncDefTable::operator[](sym::FuncId id) const -> const FuncDef& {
  auto itr = m_funcs.find(id);
  if (itr == m_funcs.end()) {
    throw std::invalid_argument{"No definition has been registered for given function id"};
  }
  return itr->second;
}

auto FuncDefTable::begin() const -> Iterator { return m_funcs.begin(); }

auto FuncDefTable::end() const -> Iterator { return m_funcs.end(); }

auto FuncDefTable::registerFunc(
    const sym::FuncDeclTable& funcTable,
    sym::FuncId id,
    sym::ConstDeclTable consts,
    expr::NodePtr expr) -> void {
  if (expr == nullptr) {
    throw std::invalid_argument{"Function expresion cannot be null"};
  }
  if (expr->getType() != funcTable[id].getOutput()) {
    throw std::invalid_argument{"Type of expresion does not match function output type"};
  }
  auto itr = m_funcs.find(id);
  if (itr != m_funcs.end()) {
    throw std::logic_error{"Function already has a definition registered"};
  }
  m_funcs.insert({id, FuncDef{id, std::move(consts), std::move(expr)}});
}

} // namespace prog::sym
