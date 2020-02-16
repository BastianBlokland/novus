#include "internal/implicit_conv.hpp"
#include "prog/program.hpp"

namespace prog::internal {

template <typename DeclTable>
auto findOverload(
    const Program& prog,
    const DeclTable& declTable,
    const std::vector<typename DeclTable::id>& overloads,
    const sym::TypeSet& input,
    prog::sym::OverloadOptions options) -> std::optional<typename DeclTable::id> {

  std::optional<typename DeclTable::id> result = std::nullopt;
  auto resultConvAmount                        = std::numeric_limits<int>::max();

  for (const auto& overload : overloads) {
    const auto ovInput = declTable[overload].getInput();
    if (ovInput.getCount() != input.getCount()) {
      continue; // Argument count has to match.
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

    auto convAmount = 0;
    auto valid      = true;
    auto inputItr   = input.begin();
    auto ovInputItr = ovInput.begin();
    for (; inputItr != input.end(); ++inputItr, ++ovInputItr) {
      if (*inputItr == *ovInputItr) {
        continue;
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
template std::optional<sym::FuncDeclTable::id> findOverload(
    const Program& prog,
    const sym::FuncDeclTable& declTable,
    const std::vector<sym::FuncId>& overloads,
    const sym::TypeSet& input,
    prog::sym::OverloadOptions options);

} // namespace prog::internal
