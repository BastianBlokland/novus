#include "prog/sym/func_def.hpp"

namespace prog::sym {

FuncDef::FuncDef(sym::FuncId id, sym::ConstDeclTable consts, expr::NodePtr expr) :
    m_id{id}, m_consts{std::move(consts)}, m_expr{std::move(expr)} {}

} // namespace prog::sym
