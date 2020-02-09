#include "prog/sym/future_def.hpp"

namespace prog::sym {

FutureDef::FutureDef(sym::TypeId id, TypeId result) : m_id{id}, m_result{result} {}

auto FutureDef::getId() const noexcept -> const TypeId& { return m_id; }

auto FutureDef::getResult() const -> const TypeId& { return m_result; }

} // namespace prog::sym
