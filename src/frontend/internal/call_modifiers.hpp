#pragma once
#include "internal/context.hpp"
#include "lex/token.hpp"
#include "parse/nodes.hpp"
#include "prog/expr/nodes.hpp"
#include <utility>
#include <vector>

namespace frontend::internal {

auto modifyCallArgs(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    const lex::Token& nameToken,
    std::optional<parse::TypeParamList> typeParams,
    const parse::CallExprNode& node,
    std::pair<std::vector<prog::expr::NodePtr>, prog::sym::TypeSet>& args) -> void;

auto modifyCallPossibleFuncs(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    const lex::Token& nameToken,
    std::optional<parse::TypeParamList> typeParams,
    const parse::CallExprNode& node,
    const std::pair<std::vector<prog::expr::NodePtr>, prog::sym::TypeSet>& args,
    std::vector<prog::sym::FuncId>& possibleFuncs) -> void;

} // namespace frontend::internal
