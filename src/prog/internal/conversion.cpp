#include "internal/conversion.hpp"
#include "prog/expr/node_call.hpp"
#include "prog/program.hpp"
#include <limits>
#include <stdexcept>

namespace prog::internal {

auto findConversion(const Program& prog, sym::TypeId from, sym::TypeId to)
    -> std::optional<sym::FuncId> {
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

auto applyConversions(
    const Program& prog, const sym::Input& input, std::vector<expr::NodePtr>* args) -> void {
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

} // namespace prog::internal
