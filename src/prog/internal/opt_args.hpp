#pragma once
#include "prog/program.hpp"

namespace prog::internal {

// Add optional argument intializers to the given args to make func callable.
auto applyOptArgIntializers(const Program& prog, sym::FuncId func, std::vector<expr::NodePtr>* args)
    -> void;

// Add optional argument intializers to all calls that do not provide overrides.
auto applyOptArgIntializers(const Program& prog) -> void;

} // namespace prog::internal
