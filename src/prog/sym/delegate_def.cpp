#include "prog/sym/delegate_def.hpp"

namespace prog::sym {

DelegateDef::DelegateDef(sym::TypeId id, bool isAction, TypeSet input, TypeId output) :
    m_id{id}, m_isAction{isAction}, m_input{std::move(input)}, m_output{output} {}

auto DelegateDef::getId() const noexcept -> const TypeId& { return m_id; }

auto DelegateDef::isAction() const noexcept -> bool { return m_isAction; }

auto DelegateDef::getInput() const -> const TypeSet& { return m_input; }

auto DelegateDef::getOutput() const -> TypeId { return m_output; }

} // namespace prog::sym
