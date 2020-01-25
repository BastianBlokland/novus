#pragma once
#include "prog/program.hpp"

namespace prog::internal {

[[nodiscard]] auto findConversion(const Program& prog, sym::TypeId from, sym::TypeId to)
    -> std::optional<sym::FuncId>;

// Note: Can be expensive as it involves walking all function declarations.
[[nodiscard]] auto findConvertibleTypes(const Program& prog, sym::TypeId from)
    -> std::vector<sym::TypeId>;

// Are the fromTypes convertible to the toTypes.
[[nodiscard]] auto
isConvertable(const Program& prog, const sym::TypeSet& toTypes, const sym::TypeSet& fromTypes)
    -> bool;

// Are the fromArgs convertible to the toTypes.
[[nodiscard]] auto isConvertable(
    const Program& prog, const sym::TypeSet& toTypes, const std::vector<expr::NodePtr>& fromArgs)
    -> bool;

// Apply conversions to the fromArgs so are matches the toTypes.
auto applyConversions(
    const Program& prog, const sym::TypeSet& toTypes, std::vector<expr::NodePtr>* fromArgs) -> void;

[[nodiscard]] auto findCommonType(const Program& prog, const std::vector<sym::TypeId>& types)
    -> std::optional<sym::TypeId>;

} // namespace prog::internal
