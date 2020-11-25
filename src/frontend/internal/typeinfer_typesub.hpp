#pragma once
#include "internal/utilities.hpp"
#include "parse/type_param_list.hpp"
#include <utility>

namespace frontend::internal {

using TypePath = typename std::vector<std::pair<std::string, unsigned int>>;

[[nodiscard]] auto
getPathsToTypeSub(const std::string& subType, const parse::Type& parsetype, const TypePath& path)
    -> std::vector<TypePath>;

[[nodiscard]] auto resolvePathToTypeSub(
    const Context& ctx,
    TypePath::const_iterator begin,
    const TypePath::const_iterator& end,
    prog::sym::TypeId type) -> std::optional<prog::sym::TypeId>;

template <typename TypeSpec>
[[nodiscard]] auto inferSubTypeFromSpecs(
    const Context& ctx,
    const std::string& subType,
    const std::vector<TypeSpec>& typeSpecs,
    const prog::sym::TypeSet& inputTypes) -> std::optional<prog::sym::TypeId> {

  if (typeSpecs.size() != inputTypes.getCount()) {
    throw std::invalid_argument{"Amount of type specifications and input types have to match"};
  }

  /* Given a set of type specifications (like a argument list of a function or a field list of a
  struct) this will attempt to infer the type of the given substitution based on a set of input
  types. */

  std::optional<prog::sym::TypeId> result = std::nullopt;
  for (auto typeInd = 0U; typeInd != typeSpecs.size(); ++typeInd) {
    const auto& typeSpec = typeSpecs[typeInd];
    for (const auto& path : getPathsToTypeSub(subType, typeSpec.getType(), {})) {
      const auto& inferredType =
          resolvePathToTypeSub(ctx, path.begin(), path.end(), inputTypes[typeInd]);
      if (!inferredType) {
        return std::nullopt;
      }
      if (!result) {
        result = inferredType;
      } else if (result != inferredType) {
        return std::nullopt;
      }
    }
  }
  return result;
}

/* Calculate the complexity of a type, used to prefer simpler types for type inference.
 * Note: Currently this is based on the amount of type-parameters and their complexity.
 */
auto getComplexity(const Context& ctx, prog::sym::TypeId type) noexcept -> int;

/* Calculate the complexity of a set of types, used to prefer simpler types for type inference.
 * Note: Currently this a sum of the complexity of the types.
 */
auto getComplexity(const Context& ctx, const prog::sym::TypeSet& types) noexcept -> int;

} // namespace frontend::internal
