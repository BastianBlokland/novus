#pragma once
#include "internal/context.hpp"
#include "lex/token.hpp"
#include "parse/nodes.hpp"
#include "prog/expr/nodes.hpp"
#include <utility>
#include <vector>

namespace frontend::internal {

auto modifyCall(
    const Context* ctx,
    const lex::Token& nameToken,
    const parse::CallExprNode& node,
    std::pair<std::vector<prog::expr::NodePtr>, prog::sym::TypeSet>* args) -> void;

} // namespace frontend::internal
