#pragma once
#include "prog/sym/type_id.hpp"
#include <string>
#include <vector>

namespace prog::sym {

class StructDef final {
  friend class TypeDefTable;

public:
  class Field final {
  public:
    Field(sym::TypeId type, std::string identifier);

    auto operator==(const Field& rhs) const noexcept -> bool;

    [[nodiscard]] auto getType() const noexcept -> const sym::TypeId&;
    [[nodiscard]] auto getIdentifier() const noexcept -> const std::string&;

  private:
    sym::TypeId m_type;
    std::string m_identifier;
  };

  using iterator = typename std::vector<Field>::const_iterator;

  StructDef()                         = delete;
  StructDef(const StructDef& rhs)     = delete;
  StructDef(StructDef&& rhs) noexcept = default;
  ~StructDef()                        = default;

  auto operator=(const StructDef& rhs) -> StructDef& = delete;
  auto operator=(StructDef&& rhs) noexcept -> StructDef& = delete;

  [[nodiscard]] auto getCount() const noexcept -> unsigned int;

  [[nodiscard]] auto begin() const -> iterator;
  [[nodiscard]] auto end() const -> iterator;

  [[nodiscard]] auto getId() const noexcept -> const TypeId&;

private:
  sym::TypeId m_id;
  std::vector<Field> m_fields;

  StructDef(sym::TypeId id, std::vector<Field> fields);
};

} // namespace prog::sym
