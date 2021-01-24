#include "prog/sym/func_def.hpp"

namespace prog::sym {

FuncDef::FuncDef(
    sym::FuncId id,
    sym::ConstDeclTable consts,
    expr::NodePtr body,
    std::vector<expr::NodePtr> optArgInitializers) :
    m_id{id},
    m_consts{std::move(consts)},
    m_body{std::move(body)},
    m_optArgInitializers{std::move(optArgInitializers)} {}

auto FuncDef::getId() const noexcept -> const FuncId& { return m_id; }

auto FuncDef::getConsts() const noexcept -> const sym::ConstDeclTable& { return m_consts; }

auto FuncDef::getBody() const noexcept -> const expr::Node& { return *m_body; }

} // namespace prog::sym
