#pragma once
#include "prog/program.hpp"
#include "prog/sym/const_decl_table.hpp"
#include <functional>
#include <memory>

namespace prog {

using RewriterFactory = typename std::function<std::unique_ptr<expr::Rewriter>(
    const Program& prog, sym::FuncId funcId, sym::ConstDeclTable* consts)>;

auto copyType(const Program& from, Program* to, sym::TypeId id) -> bool;

auto copyFunc(const Program& from, Program* to, sym::FuncId id) -> bool;

auto copyFunc(
    const Program& from,
    Program* to,
    sym::FuncId id,
    bool& modified,
    const RewriterFactory& rewriterFactory) -> bool;

} // namespace prog
