#include "prog/sym/func_def_table.hpp"
#include <stdexcept>

namespace prog::sym {

auto FuncDefTable::operator[](sym::FuncId id) const -> const FuncDef& {
  const auto index = id.m_id;
  assert(index < this->m_funcs.size());
  return m_funcs[index];
}

auto FuncDefTable::begin() const -> iterator { return m_funcs.begin(); }

auto FuncDefTable::end() const -> iterator { return m_funcs.end(); }

auto FuncDefTable::registerFunc(
    const sym::FuncDeclTable& funcTable,
    sym::FuncId id,
    sym::ConstDeclTable consts,
    expr::NodePtr expr) -> void {
  if (expr == nullptr) {
    throw std::invalid_argument{"Function expresion cannot be null"};
  }
  const auto sig = funcTable[id].getSig();
  if (expr->getType() != sig.getOutput()) {
    throw std::invalid_argument{"Type of expresion does not match function return type"};
  }
  m_funcs.push_back(FuncDef{id, std::move(consts), std::move(expr)});
}

} // namespace prog::sym
