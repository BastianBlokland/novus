#pragma once
#include "prog/program.hpp"
#include "prog/sym/const_decl_table.hpp"
#include <functional>
#include <memory>

namespace prog {

// Callback to construct a expr::rewriter for a specific function.
// Usefull for rewriting functions during the optimization phase.
using RewriterFactory = typename std::function<std::unique_ptr<expr::Rewriter>(
    const Program& prog, sym::FuncId funcId, sym::ConstDeclTable* consts)>;

// Copy the given type declaration (and definition) from a program to another.
// Return value indicates if the type was copied successfully.
auto copyType(const Program& from, Program* to, sym::TypeId id) -> bool;

// Copy the given function declaration (and definition) from a program to another.
// Return value indicates if the function was copied successfully.
auto copyFunc(const Program& from, Program* to, sym::FuncId id) -> bool;

// Copy the given function from a program to another while optionally rewriting the body expression.
// Return value indicates if the function was copied successfully.
// Note the 'modified' output parameter indicates that a rewrite took place.
auto copyFunc(
    const Program& from,
    Program* to,
    sym::FuncId id,
    bool& modified,
    const RewriterFactory& rewriterFactory) -> bool;

} // namespace prog
