#include "prog/sym/func_decl_table.hpp"
#include "internal/overload.hpp"
#include <cassert>
#include <stdexcept>

namespace prog::sym {

auto FuncDeclTable::operator[](FuncId id) const -> const FuncDecl& {
  const auto index = id.m_id;
  assert(index < this->m_funcs.size());
  return m_funcs[index];
}

auto FuncDeclTable::getFuncCount() const -> unsigned int { return m_funcs.size(); }

auto FuncDeclTable::begin() const -> iterator { return m_funcs.begin(); }

auto FuncDeclTable::end() const -> iterator { return m_funcs.end(); }

auto FuncDeclTable::exists(const std::string& name) const -> bool {
  return m_lookup.find(name) != m_lookup.end();
}

auto FuncDeclTable::lookup(const std::string& name, OverloadOptions options) const
    -> std::vector<FuncId> {
  const auto itr = m_lookup.find(name);
  if (itr == m_lookup.end()) {
    return {};
  }

  auto result = std::vector<FuncId>{};
  result.reserve(itr->second.size());
  for (const auto& f : itr->second) {
    if (options.hasFlag<OverloadFlags::ExclActions>() && m_funcs[f.m_id].isAction()) {
      continue;
    }
    if (options.hasFlag<OverloadFlags::ExclPureFuncs>() && !m_funcs[f.m_id].isAction()) {
      continue;
    }
    if (options.hasFlag<OverloadFlags::ExclNonUser>() && m_funcs[f.m_id].m_kind != FuncKind::User) {
      continue;
    }
    result.push_back(f);
  }

  return result;
}

auto FuncDeclTable::lookup(
    const Program& prog,
    const std::string& name,
    const TypeSet& input,
    OverloadOptions options) const -> std::optional<FuncId> {
  return internal::findOverload(prog, *this, lookup(name, options), input, options);
}

auto FuncDeclTable::registerImplicitConv(
    const Program& prog, FuncKind kind, std::string name, TypeSet input, TypeId output) -> FuncId {
  return registerFunc(prog, kind, false, true, std::move(name), std::move(input), output);
}

auto FuncDeclTable::registerFunc(
    const Program& prog, FuncKind kind, std::string name, TypeSet input, TypeId output) -> FuncId {
  return registerFunc(prog, kind, false, false, std::move(name), std::move(input), output);
}

auto FuncDeclTable::registerAction(
    const Program& prog, FuncKind kind, std::string name, TypeSet input, TypeId output) -> FuncId {
  return registerFunc(prog, kind, true, false, std::move(name), std::move(input), output);
}

auto FuncDeclTable::updateFuncOutput(FuncId id, TypeId newOutput) -> void {
  const auto index = id.m_id;
  assert(index < this->m_funcs.size());
  m_funcs[index].updateOutput(newOutput);
}

auto FuncDeclTable::registerFunc(
    const Program& prog,
    FuncKind kind,
    bool isAction,
    bool isImplicitConv,
    std::string name,
    TypeSet input,
    TypeId output) -> FuncId {

  if (name.empty()) {
    throw std::invalid_argument{"Name has to contain aleast 1 char"};
  }
  if (internal::findOverload(
          prog, *this, lookup(name, OverloadOptions{0}), input, OverloadOptions{0})) {
    throw std::logic_error{"Function with an identical name and input has already been registered"};
  }

  auto id  = FuncId{static_cast<unsigned int>(m_funcs.size())};
  auto itr = m_lookup.find(name);
  if (itr == m_lookup.end()) {
    itr = m_lookup.insert({name, std::vector<FuncId>{}}).first;
  }
  itr->second.push_back(id);
  m_funcs.push_back(
      FuncDecl{id, kind, isAction, isImplicitConv, std::move(name), std::move(input), output});
  return id;
}

} // namespace prog::sym
