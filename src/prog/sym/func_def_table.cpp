#include "prog/sym/func_def_table.hpp"
#include "expr/utilities.hpp"
#include <stdexcept>

namespace prog::sym {

auto FuncDefTable::operator[](sym::FuncId id) const -> const FuncDef& {
  auto itr = m_funcDefs.find(id);
  if (itr == m_funcDefs.end()) {
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
    expr::NodePtr body,
    std::vector<expr::NodePtr> optArgInitializers,
    FuncDef::Flags flags) -> void {

  if (body == nullptr) {
    throw std::invalid_argument{"Function body expresion cannot be null"};
  }
  if (body->getType() != funcTable[id].getOutput()) {
    throw std::invalid_argument{"Type of body expresion does not match function output type"};
  }

  if (expr::anyNodeNull(optArgInitializers)) {
    throw std::invalid_argument{"Optional argument intializer cannot be null"};
  }
  if (optArgInitializers.size() != funcTable[id].getNumOptInputs()) {
    throw std::invalid_argument{"Incorrect number of optional argument initializers provided"};
  }
  const auto inputConsts      = consts.getInputs();
  const auto nonOptInputCount = inputConsts.size() - funcTable[id].getNumOptInputs();
  for (auto i = 0u; i != optArgInitializers.size(); ++i) {
    const auto inputConstId = inputConsts[nonOptInputCount + i];
    if (consts[inputConstId].getType() != optArgInitializers[i]->getType()) {
      throw std::invalid_argument{"Optional argument initializer returns an incorrect type"};
    }
  }

  auto itr = m_funcDefs.find(id);
  if (itr != m_funcDefs.end()) {
    throw std::logic_error{"Function already has a definition registered"};
  }
  m_funcDefs.insert(
      {id, FuncDef{id, std::move(consts), std::move(body), std::move(optArgInitializers), flags}});
  m_funcs.insert(id);
}

} // namespace prog::sym
