#include "prog/sym/lazy_def.hpp"

namespace prog::sym {

LazyDef::LazyDef(sym::TypeId id, TypeId result, bool isAction) :
    m_id{id}, m_result{result}, m_isAction{isAction} {}

auto LazyDef::getId() const noexcept -> const TypeId& { return m_id; }

auto LazyDef::getResult() const noexcept -> const TypeId& { return m_result; }

auto LazyDef::isAction() const noexcept -> bool { return m_isAction; }

} // namespace prog::sym
