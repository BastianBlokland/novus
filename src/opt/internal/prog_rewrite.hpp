#pragma once
#include "prog/copy.hpp"
#include "prog/program.hpp"

namespace opt::internal {

// Rewrite all the functions to a new program.
auto rewrite(const prog::Program& prog, const prog::RewriterFactory& rewriterFactory)
    -> prog::Program;

} // namespace opt::internal
