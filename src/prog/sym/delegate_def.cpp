#include "prog/sym/delegate_def.hpp"

namespace prog::sym {

DelegateDef::DelegateDef(sym::TypeId id, TypeSet input, TypeId output) :
    m_id{id}, m_input{std::move(input)}, m_output{output} {}

auto DelegateDef::getId() const noexcept -> const TypeId& { return m_id; }

auto DelegateDef::getInput() const -> const TypeSet& { return m_input; }

auto DelegateDef::getOutput() const -> TypeId { return m_output; }

} // namespace prog::sym
