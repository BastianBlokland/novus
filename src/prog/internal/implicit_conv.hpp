#pragma once
#include "prog/program.hpp"

namespace prog::internal {

[[nodiscard]] auto findImplicitConv(const Program& prog, sym::TypeId from, sym::TypeId to)
    -> std::optional<sym::FuncId>;

// Note: Can be expensive as it involves walking all function declarations.
[[nodiscard]] auto findImplicitConvTypes(const Program& prog, sym::TypeId from)
    -> std::vector<sym::TypeId>;

// Are the fromTypes convertible to the toTypes.
[[nodiscard]] auto isImplicitConvertible(
    const Program& prog,
    const sym::TypeSet& toTypes,
    const sym::TypeSet& fromTypes,
    int maxConversions = -1) -> bool;

// Apply conversions to the fromArgs so are matches the toTypes.
auto applyImplicitConversions(
    const Program& prog, const sym::TypeSet& toTypes, std::vector<expr::NodePtr>* fromArgs) -> void;

[[nodiscard]] auto findCommonType(const Program& prog, const std::vector<sym::TypeId>& types)
    -> std::optional<sym::TypeId>;

} // namespace prog::internal
