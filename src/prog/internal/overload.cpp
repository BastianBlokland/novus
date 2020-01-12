#include "internal/conversion.hpp"
#include "prog/program.hpp"

namespace prog::internal {

template <typename DeclTable>
auto findOverload(
    const Program& prog,
    const DeclTable& declTable,
    const std::vector<typename DeclTable::id>& overloads,
    const sym::TypeSet& input,
    int maxConversions,
    bool allowConvOnFirstArg) -> std::optional<typename DeclTable::id> {

  std::optional<typename DeclTable::id> result = std::nullopt;
  auto resultConvAmount                        = std::numeric_limits<int>::max();

  for (const auto& overload : overloads) {
    const auto ovInput = declTable[overload].getInput();
    if (ovInput.getCount() != input.getCount()) {
      continue; // Argument count has to match.
    }

    auto convAmount = 0;
    auto valid      = true;
    auto inputItr   = input.begin();
    auto ovInputItr = ovInput.begin();
    for (; inputItr != input.end(); ++inputItr, ++ovInputItr) {
      if (*inputItr == *ovInputItr) {
        continue;
      }
      if (inputItr == input.begin() && !allowConvOnFirstArg) {
        valid = false; // Conversions are not allowed on the first argument.
        break;
      }
      if (maxConversions >= 0 && convAmount >= maxConversions) {
        valid = false; // Too many conversions are required for this overload.
        break;
      }

      auto conv = findConversion(prog, *inputItr, *ovInputItr);
      if (conv) {
        convAmount++;
        continue;
      }
      valid = false; // Type's don't match and no conversion is possible.
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
    int maxConversions,
    bool allowConvOnFirstArg);
template std::optional<sym::ActionDeclTable::id> findOverload(
    const Program& prog,
    const sym::ActionDeclTable& declTable,
    const std::vector<sym::ActionId>& overloads,
    const sym::TypeSet& input,
    int maxConversions,
    bool allowConvOnFirstArg);

} // namespace prog::internal
