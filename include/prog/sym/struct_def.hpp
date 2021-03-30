#pragma once
#include "prog/sym/field_decl_table.hpp"
#include "prog/sym/type_id.hpp"
#include <string>
#include <vector>

namespace prog::sym {

// Struct definition. Contains the fields in the structure.
class StructDef final {
  friend class TypeDefTable;

public:
  StructDef()                         = delete;
  StructDef(const StructDef& rhs)     = default;
  StructDef(StructDef&& rhs) noexcept = default;
  ~StructDef()                        = default;

  auto operator=(const StructDef& rhs) -> StructDef& = delete;
  auto operator=(StructDef&& rhs) noexcept -> StructDef& = delete;

  [[nodiscard]] auto getId() const noexcept -> const TypeId& { return m_id; }
  [[nodiscard]] auto getFields() const noexcept -> const sym::FieldDeclTable& { return m_fields; }
  [[nodiscard]] auto isTagType() const noexcept -> bool { return m_fields.getCount() == 0; }

private:
  sym::TypeId m_id;
  sym::FieldDeclTable m_fields;

  StructDef(sym::TypeId id, sym::FieldDeclTable fields) : m_id{id}, m_fields{std::move(fields)} {}
};

} // namespace prog::sym
