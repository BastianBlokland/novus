#pragma once
#include "prog/program.hpp"

namespace opt {

// Perform all optimizations on a program.
auto optimize(const prog::Program& prog) -> prog::Program;

// Remove unused functions and types from a program.
auto treeshake(const prog::Program& prog) -> prog::Program;

// Inline function bodies into the callsites.
auto inlineCalls(const prog::Program& prog) -> prog::Program;
auto inlineCalls(const prog::Program& prog, bool& modified) -> prog::Program;

// Remove unneeded constants.
// * Unused constants are removed.
// * Constants which are used only once are removed.
// * Constants with cheap initializer expressions are removed.
//
auto eliminateConsts(const prog::Program& prog) -> prog::Program;
auto eliminateConsts(const prog::Program& prog, bool& modified) -> prog::Program;

// Precompute operations on literals, for example '1 + 41' would be collapsed to '42'.
auto precomputeLiterals(const prog::Program& prog) -> prog::Program;
auto precomputeLiterals(const prog::Program& prog, bool& modified) -> prog::Program;

} // namespace opt
