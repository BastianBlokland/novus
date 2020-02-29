#pragma once
#include "prog/sym/type_id.hpp"
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace prog::sym {

class EnumDef final {
  friend class TypeDefTable;

public:
  using Iterator = typename std::unordered_map<std::string, int32_t>::const_iterator;

  EnumDef()                       = delete;
  EnumDef(const EnumDef& rhs)     = delete;
  EnumDef(EnumDef&& rhs) noexcept = default;
  ~EnumDef()                      = default;

  auto operator=(const EnumDef& rhs) -> EnumDef& = delete;
  auto operator=(EnumDef&& rhs) noexcept -> EnumDef& = delete;

  [[nodiscard]] auto begin() const -> Iterator;
  [[nodiscard]] auto end() const -> Iterator;

  [[nodiscard]] auto getId() const noexcept -> const TypeId&;
  [[nodiscard]] auto hasEntry(const std::string& name) const noexcept -> bool;
  [[nodiscard]] auto getValue(const std::string& name) const noexcept -> std::optional<int32_t>;

private:
  sym::TypeId m_id;
  std::unordered_map<std::string, int32_t> m_entries;

  EnumDef(sym::TypeId id, std::unordered_map<std::string, int32_t> entries);
};

} // namespace prog::sym
