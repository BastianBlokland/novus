#include "prog/sym/func_decl_table.hpp"
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

auto FuncDeclTable::lookup(const std::string& name, const Input& input) const
    -> std::optional<FuncId> {
  const auto itr = m_lookup.find(name);
  if (itr == m_lookup.end()) {
    return std::nullopt;
  }
  return findOverload(itr->second, input);
}

auto FuncDeclTable::registerFunc(FuncKind kind, std::string name, FuncSig sig) -> FuncId {
  if (name.empty()) {
    throw std::invalid_argument{"Name has to contain aleast 1 char"};
  }

  auto id  = FuncId{static_cast<unsigned int>(m_funcs.size())};
  auto itr = m_lookup.find(name);
  if (itr == m_lookup.end()) {
    itr = m_lookup.insert({name, std::vector<FuncId>{}}).first;
  }
  if (findOverload(itr->second, sig.getInput())) {
    throw std::logic_error{"Function with an identical name and input has already been registered"};
  }
  itr->second.push_back(id);
  m_funcs.push_back(FuncDecl{id, kind, std::move(name), std::move(sig)});
  return id;
}

auto FuncDeclTable::updateFuncRetType(FuncId id, TypeId newRetType) -> void {
  const auto index = id.m_id;
  assert(index < this->m_funcs.size());
  m_funcs[index].updateSig(FuncSig{m_funcs[index].getSig().getInput(), newRetType});
}

auto FuncDeclTable::findOverload(const std::vector<FuncId>& overloads, const Input& input) const
    -> std::optional<FuncId> {
  for (const auto& funcId : overloads) {
    const auto& funcDecl = FuncDeclTable::operator[](funcId);
    if (funcDecl.m_sig.getInput() == input) {
      return funcId;
    }
  }
  return std::nullopt;
}

} // namespace prog::sym
