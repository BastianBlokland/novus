#include "prog/sym/func_def.hpp"
#include <cassert>

namespace prog::sym {

FuncDef::FuncDef(
    sym::FuncId id,
    sym::ConstDeclTable consts,
    expr::NodePtr body,
    std::vector<expr::NodePtr> optArgInitializers,
    Flags flags) :
    m_id{id},
    m_consts{std::move(consts)},
    m_body{std::move(body)},
    m_optArgInitializers{std::move(optArgInitializers)},
    m_flags{flags} {}

auto FuncDef::getId() const noexcept -> const FuncId& { return m_id; }

auto FuncDef::getFlags() const noexcept -> Flags { return m_flags; };

auto FuncDef::hasFlags(Flags flags) const noexcept -> bool {
  const auto raw = static_cast<unsigned int>(flags);
  return (static_cast<unsigned int>(m_flags) & raw) == raw;
}

auto FuncDef::getConsts() const noexcept -> const sym::ConstDeclTable& { return m_consts; }

auto FuncDef::getBody() const noexcept -> const expr::Node& { return *m_body; }

auto FuncDef::getOptArgInitializer(unsigned int i) const -> expr::NodePtr {
  assert(i < m_optArgInitializers.size());
  return m_optArgInitializers[i]->clone(nullptr);
}

} // namespace prog::sym
