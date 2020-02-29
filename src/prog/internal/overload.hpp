#pragma once
#include "prog/sym/func_id.hpp"
#include "prog/sym/overload_options.hpp"
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
    const std::vector<typename DeclTable::Id>& overloads,
    const sym::TypeSet& input,
    sym::OverloadOptions options) -> std::optional<typename DeclTable::Id>;

} // namespace internal

} // namespace prog
