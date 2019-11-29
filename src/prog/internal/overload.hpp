#pragma once
#include "prog/sym/input.hpp"
#include <optional>
#include <string>

namespace prog {

class Program;

namespace internal {

template <typename DeclTable>
auto findOverload(
    const Program& prog,
    const DeclTable& declTable,
    const std::string& name,
    const sym::Input& input,
    int maxConversions) -> std::optional<typename DeclTable::id>;

} // namespace internal

} // namespace prog
