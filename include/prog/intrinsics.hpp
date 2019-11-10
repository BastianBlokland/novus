#pragma once
#include "prog/sym/action_decl_table.hpp"
#include "prog/sym/func_decl_table.hpp"
#include "prog/sym/type_decl_table.hpp"

namespace prog {

auto registerIntrinsics(
    sym::TypeDeclTable* typeTable, sym::FuncDeclTable* funcTable, sym::ActionDeclTable* actionTable)
    -> void;

} // namespace prog
