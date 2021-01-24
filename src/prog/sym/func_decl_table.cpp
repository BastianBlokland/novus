#include "prog/sym/func_decl_table.hpp"
#include "internal/overload.hpp"
#include "prog/program.hpp"
#include "prog/sym/overload_options.hpp"
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

auto FuncDeclTable::getCount() const -> unsigned int { return m_funcs.size(); }

auto FuncDeclTable::begin() const -> Iterator { return m_funcs.begin(); }

auto FuncDeclTable::end() const -> Iterator { return m_funcs.end(); }

auto FuncDeclTable::exists(const FuncId& id) const -> bool {
  return m_funcs.find(id) != m_funcs.end();
}

auto FuncDeclTable::exists(const std::string& name) const -> bool {
  return m_normalLookup.find(name) != m_normalLookup.end();
}

auto FuncDeclTable::lookup(const std::string& name, OverloadOptions options) const
    -> std::vector<FuncId> {
  return lookupByName(name, false, options);
}

auto FuncDeclTable::lookup(
    const Program& prog,
    const std::string& name,
    const TypeSet& input,
    OverloadOptions options) const -> std::optional<FuncId> {
  return internal::findOverload(prog, *this, lookupByName(name, false, options), input, options);
}

auto FuncDeclTable::lookupIntrinsic(const std::string& name, OverloadOptions options) const
    -> std::vector<FuncId> {
  return lookupByName(name, true, options);
}

auto FuncDeclTable::lookupIntrinsic(
    const Program& prog,
    const std::string& name,
    const TypeSet& input,
    OverloadOptions options) const -> std::optional<FuncId> {
  return internal::findOverload(prog, *this, lookupByName(name, true, options), input, options);
}

auto FuncDeclTable::registerImplicitConv(
    const Program& prog, FuncKind kind, TypeId input, TypeId output) -> FuncId {

  auto name = prog.getTypeDecl(output).getName();
  return registerFunc(prog, kind, false, false, true, std::move(name), TypeSet{input}, output, 0u);
}

auto FuncDeclTable::registerFunc(
    const Program& prog,
    FuncKind kind,
    std::string name,
    TypeSet input,
    TypeId output,
    unsigned int numOptArgs) -> FuncId {
  return registerFunc(
      prog, kind, false, false, false, std::move(name), std::move(input), output, numOptArgs);
}

auto FuncDeclTable::registerAction(
    const Program& prog,
    FuncKind kind,
    std::string name,
    TypeSet input,
    TypeId output,
    unsigned int numOptArgs) -> FuncId {
  return registerFunc(
      prog, kind, true, false, false, std::move(name), std::move(input), output, numOptArgs);
}

auto FuncDeclTable::registerIntrinsic(
    const Program& prog, FuncKind kind, std::string name, TypeSet input, TypeId output) -> FuncId {
  return registerFunc(
      prog, kind, false, true, false, std::move(name), std::move(input), output, 0u);
}

auto FuncDeclTable::registerIntrinsicAction(
    const Program& prog, FuncKind kind, std::string name, TypeSet input, TypeId output) -> FuncId {
  return registerFunc(prog, kind, true, true, false, std::move(name), std::move(input), output, 0u);
}

auto FuncDeclTable::insertFunc(
    FuncId id,
    FuncKind kind,
    bool isAction,
    bool isIntrinsic,
    bool isImplicitConv,
    std::string name,
    TypeSet input,
    TypeId output,
    unsigned int numOptArgs) -> void {

  if (name.empty()) {
    throw std::invalid_argument{"Name has to contain aleast 1 char"};
  }

  // Insert into function map.
  auto funcDecl = FuncDecl{
      id, kind, isAction, isIntrinsic, isImplicitConv, name, std::move(input), output, numOptArgs};
  if (!m_funcs.insert({id, std::move(funcDecl)}).second) {
    throw std::invalid_argument{"There is already a function registered with the same id"};
  }

  // Register in the lookup map.
  (isIntrinsic ? m_intrinsicLookup : m_normalLookup).insert({std::move(name), id});
}

auto FuncDeclTable::updateFuncOutput(FuncId id, TypeId newOutput) -> void {
  const auto itr = m_funcs.find(id);
  if (itr == m_funcs.end()) {
    throw std::invalid_argument{"No declaration found for given func-id"};
  }
  itr->second.updateOutput(newOutput);
}

auto FuncDeclTable::lookupByName(
    const std::string& name, bool intrinsic, OverloadOptions options) const -> std::vector<FuncId> {
  const auto mapRange = (intrinsic ? m_intrinsicLookup : m_normalLookup).equal_range(name);

  auto result = std::vector<FuncId>{};
  result.reserve(std::distance(mapRange.first, mapRange.second));

  for (auto itr = mapRange.first; itr != mapRange.second; ++itr) {
    const auto funcId = itr->second;
    const auto& decl  = m_funcs.find(funcId)->second;
    if (options.hasFlag<OverloadFlags::ExclActions>() && decl.isAction()) {
      continue;
    }
    if (options.hasFlag<OverloadFlags::ExclPureFuncs>() && !decl.isAction()) {
      continue;
    }
    if (options.hasFlag<OverloadFlags::ExclNonUser>() && decl.m_kind != FuncKind::User) {
      continue;
    }
    result.push_back(funcId);
  }

  return result;
}

auto FuncDeclTable::registerFunc(
    const Program& prog,
    FuncKind kind,
    bool isAction,
    bool isIntrinsic,
    bool isImplicitConv,
    std::string name,
    TypeSet input,
    TypeId output,
    unsigned int numOptArgs) -> FuncId {

  if (name.empty()) {
    throw std::invalid_argument{"Name has to contain aleast 1 char"};
  }
  if (internal::findOverload(
          prog,
          *this,
          lookupByName(name, isIntrinsic, OverloadOptions{0}),
          input,
          OverloadOptions{0})) {
    throw std::logic_error{"Function with an identical name and input has already been registered"};
  }

  // Assign an id one higher then the current highest, starting from 0.
  const auto highestKey = m_funcs.rbegin();
  const auto id         = FuncId{highestKey == m_funcs.rend() ? 0 : highestKey->first.m_id + 1};

  (isIntrinsic ? m_intrinsicLookup : m_normalLookup).insert({name, id});

  auto funcDecl = FuncDecl{
      id,
      kind,
      isAction,
      isIntrinsic,
      isImplicitConv,
      std::move(name),
      std::move(input),
      output,
      numOptArgs};
  m_funcs.insert({id, std::move(funcDecl)});
  return id;
}

} // namespace prog::sym
