#pragma once
#include <iostream>

namespace prog::sym {

// Identifier for a field, wrapper around an unsigned int.
class FieldId final {
  friend class FieldDeclTable;
  friend auto operator<<(std::ostream& out, const FieldId& rhs) -> std::ostream&;

public:
  auto operator==(const FieldId& rhs) const noexcept -> bool { return m_id == rhs.m_id; }
  auto operator!=(const FieldId& rhs) const noexcept -> bool { return !FieldId::operator==(rhs); }

  [[nodiscard]] auto getNum() const noexcept -> unsigned int { return m_id; }

private:
  unsigned int m_id;

  explicit FieldId(unsigned int id) : m_id{id} {}
};

inline auto operator<<(std::ostream& out, const FieldId& rhs) -> std::ostream& {
  return out << "field-" << rhs.m_id;
}

} // namespace prog::sym
