#include "prog/sym/func_decl_table.hpp"
#include "internal/overload.hpp"
#include "prog/program.hpp"
#include <cassert>
#include <stdexcept>

namespace prog::sym {

auto FuncDeclTable::operator[](FuncId id) const -> const FuncDecl& {
  const auto itr = m_funcs.find(id);
  if (itr == m_funcs.end()) {
    throw std::invalid_argument{"No declaration found for given func-id"};
  }
  return itr->second;
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
    const auto& decl = m_funcs.find(f)->second;
    if (options.hasFlag<OverloadFlags::ExclActions>() && decl.isAction()) {
      continue;
    }
    if (options.hasFlag<OverloadFlags::ExclPureFuncs>() && !decl.isAction()) {
      continue;
    }
    if (options.hasFlag<OverloadFlags::ExclNonUser>() && decl.m_kind != FuncKind::User) {
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
    const Program& prog, FuncKind kind, TypeId input, TypeId output) -> FuncId {

  auto name = prog.getTypeDecl(output).getName();
  return registerFunc(prog, kind, false, true, std::move(name), TypeSet{input}, output);
}

auto FuncDeclTable::registerFunc(
    const Program& prog, FuncKind kind, std::string name, TypeSet input, TypeId output) -> FuncId {
  return registerFunc(prog, kind, false, false, std::move(name), std::move(input), output);
}

auto FuncDeclTable::registerAction(
    const Program& prog, FuncKind kind, std::string name, TypeSet input, TypeId output) -> FuncId {
  return registerFunc(prog, kind, true, false, std::move(name), std::move(input), output);
}

auto FuncDeclTable::insertFunc(
    FuncId id,
    FuncKind kind,
    bool isAction,
    bool isImplicitConv,
    std::string name,
    TypeSet input,
    TypeId output) -> void {

  if (name.empty()) {
    throw std::invalid_argument{"Name has to contain aleast 1 char"};
  }

  // Insert into function map.
  if (!m_funcs
           .insert(
               {id, FuncDecl{id, kind, isAction, isImplicitConv, name, std::move(input), output}})
           .second) {
    throw std::invalid_argument{"There is already a function registered with the same id"};
  }

  // Insert into name loopup map.
  auto itr = m_lookup.find(name);
  if (itr == m_lookup.end()) {
    itr = m_lookup.insert({std::move(name), std::vector<FuncId>{}}).first;
  }
  itr->second.push_back(id);
}

auto FuncDeclTable::updateFuncOutput(FuncId id, TypeId newOutput) -> void {
  const auto itr = m_funcs.find(id);
  if (itr == m_funcs.end()) {
    throw std::invalid_argument{"No declaration found for given func-id"};
  }
  itr->second.updateOutput(newOutput);
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

  // Assign an id one higher then the current highest, starting from 0.
  const auto highestKey = m_funcs.rbegin();
  const auto id         = FuncId{highestKey == m_funcs.rend() ? 0 : highestKey->first.m_id + 1};

  auto itr = m_lookup.find(name);
  if (itr == m_lookup.end()) {
    itr = m_lookup.insert({name, std::vector<FuncId>{}}).first;
  }
  itr->second.push_back(id);
  m_funcs.insert(
      {id,
       FuncDecl{id, kind, isAction, isImplicitConv, std::move(name), std::move(input), output}});
  return id;
}

} // namespace prog::sym
