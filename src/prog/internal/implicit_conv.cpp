#include "internal/implicit_conv.hpp"
#include "prog/expr/node_call.hpp"
#include "prog/program.hpp"
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <unordered_map>

namespace prog::internal {

auto findImplicitConv(const Program& prog, sym::TypeId from, sym::TypeId to)
    -> std::optional<sym::FuncId> {
  if (!from.isConcrete() || !to.isConcrete()) {
    return std::nullopt;
  }
  if (from == to) {
    throw std::logic_error{"No conversion required if types are equal"};
  }

  const auto& typeTable = internal::getTypeDeclTable(prog);
  const auto& funcTable = internal::getFuncDeclTable(prog);

  const auto& toName = typeTable[to].getName();
  for (const auto& funcId : funcTable.lookup(toName, OvOptions{OvFlags::ExclActions})) {
    const auto& funcDecl = funcTable[funcId];
    if (!funcDecl.isImplicitConv() || funcDecl.getOutput() != to) {
      continue;
    }
    const auto& funcInput = funcDecl.getInput();
    if (funcInput.getCount() == 1 && *funcInput.begin() == from) {
      return funcId;
    }
  }
  return std::nullopt;
}

auto findImplicitConvTypes(const Program& prog, sym::TypeId from) -> std::vector<sym::TypeId> {
  const auto& typeTable = internal::getTypeDeclTable(prog);
  const auto& funcTable = internal::getFuncDeclTable(prog);

  auto result = std::vector<sym::TypeId>{};

  for (const auto& funcDecl : funcTable) {
    if (!funcDecl.isImplicitConv()) {
      continue;
    }
    if (funcDecl.getInput().getCount() != 1 || *funcDecl.getInput().begin() != from) {
      continue;
    }
    if (!funcDecl.getOutput().isConcrete()) {
      continue;
    }
    if (typeTable[funcDecl.getOutput()].getName() != funcDecl.getName()) {
      continue;
    }
    result.push_back(funcDecl.getOutput());
  }

  return result;
}

auto isImplicitConvertible(
    const Program& prog, const sym::TypeSet& toTypes, const sym::TypeSet& fromTypes) -> bool {
  if (toTypes.getCount() != fromTypes.getCount()) {
    return false;
  }
  auto fromTypesItr = fromTypes.begin();
  auto toTypesItr   = toTypes.begin();
  for (; fromTypesItr != fromTypes.end(); ++fromTypesItr, ++toTypesItr) {
    if (*fromTypesItr != *toTypesItr && !findImplicitConv(prog, *fromTypesItr, *toTypesItr)) {
      return false;
    }
  }
  return true;
}

auto isImplicitConvertible(
    const Program& prog, const sym::TypeSet& toTypes, const std::vector<expr::NodePtr>& fromArgs)
    -> bool {
  if (toTypes.getCount() != fromArgs.size()) {
    return false;
  }
  auto fromArgsItr = fromArgs.begin();
  auto toTypesItr  = toTypes.begin();
  for (; fromArgsItr != fromArgs.end(); ++fromArgsItr, ++toTypesItr) {
    const auto argType = (*fromArgsItr)->getType();
    if (argType != *toTypesItr && !findImplicitConv(prog, argType, *toTypesItr)) {
      return false;
    }
  }
  return true;
}

auto applyImplicitConversions(
    const Program& prog, const sym::TypeSet& toTypes, std::vector<expr::NodePtr>* fromArgs)
    -> void {
  if (!fromArgs) {
    throw std::invalid_argument{"Null fromArgs pointer provided"};
  }
  if (toTypes.getCount() != fromArgs->size()) {
    throw std::invalid_argument{"Number of arguments does not match toTypes argument count"};
  }

  auto fromArgsItr = fromArgs->begin();
  auto toTypesItr  = toTypes.begin();
  for (; fromArgsItr != fromArgs->end(); ++fromArgsItr, ++toTypesItr) {
    const auto fromType = (*fromArgsItr)->getType();
    const auto toType   = *toTypesItr;
    if (fromType == toType) {
      continue;
    }
    auto conv = findImplicitConv(prog, fromType, toType);
    if (!conv) {
      throw std::logic_error{"No conversion possible for one of the arguments"};
    }
    auto convArgs = std::vector<expr::NodePtr>{};
    convArgs.push_back(std::move(*fromArgsItr));
    *fromArgsItr = expr::callExprNode(prog, *conv, std::move(convArgs));
  }
}

auto findCommonType(const Program& prog, const std::vector<sym::TypeId>& types)
    -> std::optional<sym::TypeId> {

  if (types.size() == 1U) {
    return types[0];
  }

  auto possible = std::unordered_map<sym::TypeId, unsigned int, sym::TypeIdHasher>{};

  // Gather all types to check (every type and what types those are convertible to).
  for (const auto& type : types) {
    possible.insert({type, 0});
    for (const auto& convertibleType : findImplicitConvTypes(prog, type)) {
      possible.insert({convertibleType, 0});
    }
  }

  // Test if the given types are convertible to those and count how many conversions it takes.
  for (auto possibleItr = possible.begin(); possibleItr != possible.end();) {
    auto erased = false;
    for (const auto& type : types) {
      if (type == possibleItr->first) {
        continue;
      }
      if (findImplicitConv(prog, type, possibleItr->first)) {
        ++possibleItr->second;
      } else {
        possibleItr = possible.erase(possibleItr);
        erased      = true;
        break;
      }
    }
    if (!erased) {
      ++possibleItr;
    }
  }

  // Return the type that required the least amount of conversions.
  auto best = std::min_element(possible.begin(), possible.end(), [](const auto& a, const auto& b) {
    return a.second < b.second;
  });
  if (best != possible.end()) {
    return best->first;
  }
  return std::nullopt;
}

} // namespace prog::internal
