#pragma once
#include <iostream>

namespace prog::sym {

// Identifier for a constant, wrapper around an unsigned int.
class ConstId final {
  friend class ConstDeclTable;
  friend class ConstIdHasher;
  friend auto operator<<(std::ostream& out, const ConstId& rhs) -> std::ostream&;

public:
  auto operator==(const ConstId& rhs) const noexcept -> bool { return m_id == rhs.m_id; }
  auto operator!=(const ConstId& rhs) const noexcept -> bool { return !ConstId::operator==(rhs); }

  auto operator<(const ConstId& rhs) const noexcept -> bool { return m_id < rhs.m_id; }
  auto operator>(const ConstId& rhs) const noexcept -> bool { return m_id > rhs.m_id; }

  [[nodiscard]] auto getNum() const noexcept -> unsigned int { return m_id; }

private:
  unsigned int m_id;

  explicit ConstId(unsigned int id) : m_id{id} {}
};

inline auto operator<<(std::ostream& out, const ConstId& rhs) -> std::ostream& {
  return out << "const-" << rhs.m_id;
}

} // namespace prog::sym
