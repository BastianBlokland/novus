#pragma once
#include "prog/sym/func_id.hpp"
#include "prog/sym/type_set.hpp"
#include <optional>
#include <string>

namespace prog {

class Program;

namespace internal {

template <typename DeclTable>
auto findOverload(
    const Program& prog,
    const DeclTable& declTable,
    const std::vector<typename DeclTable::id>& overloads,
    const sym::TypeSet& input,
    int maxConversions) -> std::optional<typename DeclTable::id>;

} // namespace internal

} // namespace prog
