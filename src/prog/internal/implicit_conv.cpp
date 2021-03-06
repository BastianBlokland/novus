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

  for (const auto& func : funcTable) {
    const auto& funcDecl = func.second;
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

auto isImplicitConvertible(const Program& prog, sym::TypeId from, sym::TypeId to) -> bool {
  if (from == to) {
    return true;
  }
  return findImplicitConv(prog, from, to).has_value();
}

auto isImplicitConvertible(
    const Program& prog,
    const sym::TypeSet& toTypes,
    const sym::TypeSet& fromTypes,
    int maxConversions,
    unsigned int numOptToTypes) -> bool {

  const auto minToTypesCount = toTypes.getCount() - numOptToTypes;
  if (fromTypes.getCount() < minToTypesCount || fromTypes.getCount() > toTypes.getCount()) {
    return false;
  }
  auto convAmount   = 0;
  auto fromTypesItr = fromTypes.begin();
  auto toTypesItr   = toTypes.begin();
  for (; fromTypesItr != fromTypes.end(); ++fromTypesItr, ++toTypesItr) {
    // Check if types match.
    if (*fromTypesItr == *toTypesItr) {
      continue;
    }

    // Check if this is not requiring too many conversions.
    if (maxConversions > 0 && convAmount >= maxConversions) {
      return false;
    }
    convAmount++;

    // Check if an conversion is possible.
    if (!findImplicitConv(prog, *fromTypesItr, *toTypesItr)) {
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
  if (fromArgs->size() > toTypes.getCount()) {
    throw std::invalid_argument{"Too many arguments provided"};
  }

  // Note: Its a valid scenario to pass less arguments then there are types in 'toTypes', reason is
  // the other types could be for optional arguments.

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
