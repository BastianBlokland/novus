#pragma once
#include "internal/context.hpp"
#include "lex/token.hpp"
#include <optional>
#include <utility>
#include <vector>

namespace frontend::internal {

using IntrinsicArgs = std::pair<std::vector<prog::expr::NodePtr>, prog::sym::TypeSet>;

auto resolveMetaIntrinsic(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    bool allowActions,
    const lex::Token& nameToken,
    const std::optional<parse::TypeParamList>& typeParams,
    IntrinsicArgs& args) -> std::optional<prog::expr::NodePtr>;

} // namespace frontend::internal
