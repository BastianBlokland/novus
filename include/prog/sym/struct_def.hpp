#pragma once
#include "prog/sym/field_decl_table.hpp"
#include "prog/sym/type_id.hpp"
#include <string>
#include <vector>

namespace prog::sym {

class StructDef final {
  friend class TypeDefTable;

public:
  StructDef()                         = delete;
  StructDef(const StructDef& rhs)     = delete;
  StructDef(StructDef&& rhs) noexcept = default;
  ~StructDef()                        = default;

  auto operator=(const StructDef& rhs) -> StructDef& = delete;
  auto operator=(StructDef&& rhs) noexcept -> StructDef& = delete;

  [[nodiscard]] auto getId() const noexcept -> const TypeId&;
  [[nodiscard]] auto getFields() const noexcept -> const sym::FieldDeclTable&;

private:
  sym::TypeId m_id;
  sym::FieldDeclTable m_fields;

  StructDef(sym::TypeId id, sym::FieldDeclTable fields);
};

} // namespace prog::sym
