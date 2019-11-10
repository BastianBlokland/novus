#include "prog/sym/func_def.hpp"

namespace prog::sym {

FuncDef::FuncDef(sym::FuncId id, sym::ConstDeclTable consts, expr::NodePtr expr) :
    m_id{id}, m_consts{std::move(consts)}, m_expr{std::move(expr)} {}

auto FuncDef::getId() const noexcept -> const FuncId& { return m_id; }

auto FuncDef::getConsts() const noexcept -> const sym::ConstDeclTable& { return m_consts; }

auto FuncDef::getExpr() const noexcept -> const expr::Node& { return *m_expr; }

} // namespace prog::sym
