#pragma once
#include "internal/context.hpp"
#include "internal/type_substitution_table.hpp"

namespace frontend::internal {

template <typename FuncParseNode>
auto defineFunc(
    Context* ctx,
    const TypeSubstitutionTable* typeSubTable,
    prog::sym::FuncId id,
    std::string funcDisplayName,
    const FuncParseNode& n) -> bool;

} // namespace frontend::internal
