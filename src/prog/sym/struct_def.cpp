#include "prog/sym/struct_def.hpp"

namespace prog::sym {

StructDef::StructDef(sym::TypeId id, sym::FieldDeclTable fields) :
    m_id{id}, m_fields{std::move(fields)} {}

auto StructDef::getId() const noexcept -> const TypeId& { return m_id; }

auto StructDef::getFields() const noexcept -> const sym::FieldDeclTable& { return m_fields; }

} // namespace prog::sym
