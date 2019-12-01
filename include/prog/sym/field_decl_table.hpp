#pragma once
#include "prog/sym/field_decl.hpp"
#include <optional>
#include <unordered_map>
#include <vector>

namespace prog::sym {

class FieldDeclTable final {
public:
  using iterator = typename std::vector<FieldDecl>::const_iterator;

  FieldDeclTable()                              = default;
  FieldDeclTable(const FieldDeclTable& rhs)     = delete;
  FieldDeclTable(FieldDeclTable&& rhs) noexcept = default;
  ~FieldDeclTable()                             = default;

  auto operator=(const FieldDeclTable& rhs) -> FieldDeclTable& = delete;
  auto operator=(FieldDeclTable&& rhs) noexcept -> FieldDeclTable& = delete;

  [[nodiscard]] auto operator[](FieldId id) const -> const FieldDecl&;

  [[nodiscard]] auto getCount() const -> unsigned int;

  [[nodiscard]] auto begin() const -> iterator;
  [[nodiscard]] auto end() const -> iterator;

  [[nodiscard]] auto lookup(const std::string& name) const -> std::optional<FieldId>;

  auto registerField(std::string name, TypeId type) -> FieldId;

private:
  std::vector<FieldDecl> m_fields;
  std::unordered_map<std::string, FieldId> m_lookup;
};

} // namespace prog::sym
