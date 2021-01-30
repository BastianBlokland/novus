#include "internal/implicit_conv.hpp"
#include "prog/program.hpp"

namespace prog::internal {

template <typename DeclTable>
auto findOverload(
    const Program& prog,
    const DeclTable& declTable,
    const std::vector<typename DeclTable::Id>& overloads,
    const sym::TypeSet& input,
    prog::sym::OverloadOptions options) -> std::optional<typename DeclTable::Id> {

  std::optional<typename DeclTable::Id> result = std::nullopt;
  auto resultConvAmount                        = std::numeric_limits<int>::max();

  for (const auto& overload : overloads) {
    const auto& overloadDecl = declTable[overload];
    if (input.getCount() < overloadDecl.getMinInputCount() ||
        input.getCount() > overloadDecl.getInput().getCount()) {
      continue;
    }

    // Check if this overload satisfies the given options.
    using Flags = sym::OverloadFlags;
    if (options.hasFlag<Flags::ExclActions>() && declTable[overload].isAction()) {
      continue;
    }
    if (options.hasFlag<Flags::ExclPureFuncs>() && !declTable[overload].isAction()) {
      continue;
    }
    if (options.hasFlag<Flags::ExclNonUser>() &&
        declTable[overload].getKind() != sym::FuncKind::User) {
      continue;
    }
    if (options.hasFlag<Flags::NoOptArgs>() &&
        input.getCount() != overloadDecl.getInput().getCount()) {
      continue;
    }

    auto convAmount = 0;
    auto valid      = true;
    auto inputItr   = input.begin();
    auto ovInputItr = overloadDecl.getInput().begin();
    for (; inputItr != input.end(); ++inputItr, ++ovInputItr) {
      if (*inputItr == *ovInputItr) {
        continue;
      }
      if (options.hasFlag<Flags::NoConvOnFirstArg>() && inputItr == input.begin()) {
        valid = false; // No conversions allowed on the first argument.
        break;
      }
      if (options.getMaxImplicitConvs() >= 0 && convAmount >= options.getMaxImplicitConvs()) {
        valid = false; // Too many implicit conversions are required for this overload.
        break;
      }

      auto conv = findImplicitConv(prog, *inputItr, *ovInputItr);
      if (conv) {
        convAmount++;
        continue;
      }
      valid = false; // Type's don't match and no implicit conversion is possible.
      break;
    }

    // Check if this overload is better (less conversions) then the previous best.
    if (valid && convAmount < resultConvAmount) {
      result           = overload;
      resultConvAmount = convAmount;
    }
  }
  return result;
}

// Explicit instantiations.
template std::optional<sym::FuncDeclTable::Id> findOverload(
    const Program& prog,
    const sym::FuncDeclTable& declTable,
    const std::vector<sym::FuncId>& overloads,
    const sym::TypeSet& input,
    prog::sym::OverloadOptions options);

} // namespace prog::internal
