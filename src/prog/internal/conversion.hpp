#pragma once
#include "prog/program.hpp"

namespace prog::internal {

[[nodiscard]] auto findConversion(const Program& prog, sym::TypeId from, sym::TypeId to)
    -> std::optional<sym::FuncId>;

// Apply conversions to the args vector so it matches the given input set.
auto applyConversions(
    const Program& prog, const sym::Input& input, std::vector<expr::NodePtr>* args) -> void;

} // namespace prog::internal
