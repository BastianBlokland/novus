#include "prog/sym/func_decl_table.hpp"
#include "internal/overload.hpp"
#include <stdexcept>

namespace prog::sym {

auto FuncDeclTable::operator[](FuncId id) const -> const FuncDecl& {
  const auto index = id.m_id;
  assert(index < this->m_funcs.size());
  return m_funcs[index];
}

auto FuncDeclTable::begin() const -> iterator { return m_funcs.begin(); }

auto FuncDeclTable::end() const -> iterator { return m_funcs.end(); }

auto FuncDeclTable::lookup(const std::string& name) const -> std::vector<FuncId> {
  const auto itr = m_lookup.find(name);
  if (itr == m_lookup.end()) {
    return std::vector<FuncId>{};
  }
  return itr->second;
}

auto FuncDeclTable::lookup(
    const Program& prog, const std::string& name, const Input& input, int maxConversions) const
    -> std::optional<FuncId> {
  return internal::findOverload(prog, *this, name, input, maxConversions);
}

auto FuncDeclTable::registerFunc(
    const Program& prog, FuncKind kind, std::string name, Input input, TypeId output) -> FuncId {
  if (name.empty()) {
    throw std::invalid_argument{"Name has to contain aleast 1 char"};
  }
  if (internal::findOverload(prog, *this, name, input, 0)) {
    throw std::logic_error{"Action with an identical name and input has already been registered"};
  }

  auto id  = FuncId{static_cast<unsigned int>(m_funcs.size())};
  auto itr = m_lookup.find(name);
  if (itr == m_lookup.end()) {
    itr = m_lookup.insert({name, std::vector<FuncId>{}}).first;
  }
  itr->second.push_back(id);
  m_funcs.push_back(FuncDecl{id, kind, std::move(name), std::move(input), output});
  return id;
}

auto FuncDeclTable::updateFuncOutput(FuncId id, TypeId newOutput) -> void {
  const auto index = id.m_id;
  assert(index < this->m_funcs.size());
  m_funcs[index].updateOutput(newOutput);
}

} // namespace prog::sym
