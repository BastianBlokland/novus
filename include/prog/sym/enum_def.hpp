#pragma once
#include "prog/sym/type_id.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace prog::sym {

// Definition of an enum type. Contains a set of values with names.
class EnumDef final {
  friend class TypeDefTable;

public:
  using Pair     = std::pair<std::string, int32_t>;
  using Iterator = std::vector<Pair>::const_iterator;

  EnumDef()                       = delete;
  EnumDef(const EnumDef& rhs)     = default;
  EnumDef(EnumDef&& rhs) noexcept = default;
  ~EnumDef()                      = default;

  auto operator=(const EnumDef& rhs) -> EnumDef& = delete;
  auto operator=(EnumDef&& rhs) noexcept -> EnumDef& = delete;

  [[nodiscard]] auto begin() const -> Iterator;
  [[nodiscard]] auto end() const -> Iterator;

  [[nodiscard]] auto operator[](size_t idx) const -> const Pair&;

  [[nodiscard]] auto getId() const noexcept -> const TypeId&;
  [[nodiscard]] auto getCount() const noexcept -> size_t;

  [[nodiscard]] auto hasEntry(const std::string& name) const noexcept -> bool;
  [[nodiscard]] auto getValue(const std::string& name) const noexcept -> std::optional<int32_t>;

private:
  sym::TypeId m_id;
  std::vector<Pair> m_entries;

  EnumDef(sym::TypeId id, std::vector<Pair> entries);
};

} // namespace prog::sym
