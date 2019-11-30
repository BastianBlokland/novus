#include "prog/sym/struct_def.hpp"

namespace prog::sym {

StructDef::Field::Field(sym::TypeId type, std::string identifier) :
    m_type{type}, m_identifier{std::move(identifier)} {}

auto StructDef::Field::operator==(const Field& rhs) const noexcept -> bool {
  return m_type == rhs.m_type && m_identifier == rhs.m_identifier;
}

auto StructDef::Field::getType() const noexcept -> const sym::TypeId& { return m_type; }

auto StructDef::Field::getIdentifier() const noexcept -> const std::string& { return m_identifier; }

StructDef::StructDef(sym::TypeId id, std::vector<Field> fields) :
    m_id{id}, m_fields{std::move(fields)} {}

auto StructDef::getCount() const noexcept -> unsigned int { return m_fields.size(); }

auto StructDef::begin() const -> iterator { return m_fields.begin(); }

auto StructDef::end() const -> iterator { return m_fields.end(); }

auto StructDef::getId() const noexcept -> const TypeId& { return m_id; }

} // namespace prog::sym
