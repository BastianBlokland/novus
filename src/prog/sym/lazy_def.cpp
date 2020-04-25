#include "prog/sym/lazy_def.hpp"

namespace prog::sym {

LazyDef::LazyDef(sym::TypeId id, TypeId result) : m_id{id}, m_result{result} {}

auto LazyDef::getId() const noexcept -> const TypeId& { return m_id; }

auto LazyDef::getResult() const -> const TypeId& { return m_result; }

} // namespace prog::sym
