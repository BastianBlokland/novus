#include "internal/conversion.hpp"
#include "prog/expr/node_call.hpp"
#include "prog/program.hpp"
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <unordered_map>

namespace prog::internal {

auto findConversion(const Program& prog, sym::TypeId from, sym::TypeId to)
    -> std::optional<sym::FuncId> {
  if (!from.isConcrete() || !to.isConcrete()) {
    return std::nullopt;
  }
  if (from == to) {
    throw std::logic_error{"No conversion required if types are equal"};
  }

  const auto& typeTable = internal::getTypeDeclTable(prog);
  const auto& funcTable = internal::getFuncDeclTable(prog);

  // Conversions are funcs with the name of the target and a single input param.
  const auto& toName = typeTable[to].getName();
  for (const auto& funcId : funcTable.lookup(toName)) {
    const auto& funcDecl = funcTable[funcId];
    if (funcDecl.getOutput() != to) {
      continue;
    }
    const auto& funcInput = funcDecl.getInput();
    if (funcInput.getCount() == 1 && *funcInput.begin() == from) {
      return funcId;
    }
  }
  return std::nullopt;
}

auto findConvertibleTypes(const Program& prog, sym::TypeId from) -> std::vector<sym::TypeId> {
  const auto& typeTable = internal::getTypeDeclTable(prog);
  const auto& funcTable = internal::getFuncDeclTable(prog);

  auto result = std::vector<sym::TypeId>{};

  // Conversions are funcs with the name of the target and a single input param.
  for (const auto& funcDecl : funcTable) {
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

auto applyConversions(
    const Program& prog, const sym::TypeSet& input, std::vector<expr::NodePtr>* args) -> void {
  if (!args) {
    throw std::invalid_argument{"Null args pointer provided"};
  }
  if (input.getCount() != args->size()) {
    throw std::invalid_argument{"Number of arguments does not match input argument count"};
  }

  auto argsItr  = args->begin();
  auto inputItr = input.begin();
  for (; argsItr != args->end(); ++argsItr, ++inputItr) {
    const auto argType   = (*argsItr)->getType();
    const auto inputType = *inputItr;
    if (argType == inputType) {
      continue;
    }
    auto conv = findConversion(prog, argType, inputType);
    if (!conv) {
      throw std::logic_error{"No conversion possible for one of the arguments"};
    }
    auto convArgs = std::vector<expr::NodePtr>{};
    convArgs.push_back(std::move(*argsItr));
    *argsItr = expr::callExprNode(prog, *conv, std::move(convArgs));
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
    for (const auto& convertibleType : findConvertibleTypes(prog, type)) {
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
      if (findConversion(prog, type, possibleItr->first)) {
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
