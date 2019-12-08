#pragma once
#include "prog/program.hpp"

namespace prog::internal {

[[nodiscard]] auto findConversion(const Program& prog, sym::TypeId from, sym::TypeId to)
    -> std::optional<sym::FuncId>;

// Note: Can be expensive as it involves walking all function declarations.
[[nodiscard]] auto findConvertibleTypes(const Program& prog, sym::TypeId from)
    -> std::vector<sym::TypeId>;

// Apply conversions to the args vector so it matches the given input set.
auto applyConversions(
    const Program& prog, const sym::Input& input, std::vector<expr::NodePtr>* args) -> void;

[[nodiscard]] auto findCommonType(const Program& prog, const std::vector<sym::TypeId>& types)
    -> std::optional<sym::TypeId>;

} // namespace prog::internal
