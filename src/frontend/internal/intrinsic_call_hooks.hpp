#pragma once
#include "internal/context.hpp"
#include "lex/token.hpp"
#include <utility>
#include <vector>

namespace frontend::internal {

auto injectPossibleIntrinsicFunctions(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    const lex::Token& nameToken,
    input::Span callSpan,
    const std::optional<parse::TypeParamList>& typeParams,
    const std::pair<std::vector<prog::expr::NodePtr>, prog::sym::TypeSet>& args,
    std::vector<prog::sym::FuncId>& possibleFuncs) -> void;

} // namespace frontend::internal
