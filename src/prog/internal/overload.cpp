#include "internal/conversion.hpp"
#include "prog/program.hpp"

namespace prog::internal {

template <typename DeclTable>
auto findOverload(
    const Program& prog,
    const DeclTable& declTable,
    const std::string& name,
    const sym::Input& input,
    int maxConversions) -> std::optional<typename DeclTable::id> {

  std::optional<typename DeclTable::id> result = std::nullopt;
  auto resultConvAmount                        = std::numeric_limits<int>::max();

  for (const auto& overload : declTable.lookup(name)) {
    const auto funcInput = declTable[overload].getInput();
    if (funcInput.getCount() != input.getCount()) {
      continue; // Argument count has to match.
    }

    auto convAmount   = 0;
    auto valid        = true;
    auto inputItr     = input.begin();
    auto funcInputItr = funcInput.begin();
    for (; inputItr != input.end(); ++inputItr, ++funcInputItr) {
      if (*inputItr == *funcInputItr) {
        continue;
      }
      if (maxConversions >= 0 && convAmount >= maxConversions) {
        valid = false; // Too many conversions are required for this overload.
        break;
      }

      auto conv = findConversion(prog, *inputItr, *funcInputItr);
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
    const std::string& name,
    const sym::Input& input,
    int maxConversions);
template std::optional<sym::ActionDeclTable::id> findOverload(
    const Program& prog,
    const sym::ActionDeclTable& declTable,
    const std::string& name,
    const sym::Input& input,
    int maxConversions);

} // namespace prog::internal
