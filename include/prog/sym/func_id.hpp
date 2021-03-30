#pragma once
#include <iostream>

namespace prog::sym {

// Identifier for a function, wrapper around an unsigned int.
class FuncId final {
  friend class FuncDeclTable;
  friend class FuncDefTable;
  friend class FuncIdHasher;
  friend auto operator<<(std::ostream& out, const FuncId& rhs) -> std::ostream&;

public:
  auto operator==(const FuncId& rhs) const noexcept -> bool { return m_id == rhs.m_id; }
  auto operator!=(const FuncId& rhs) const noexcept -> bool { return !FuncId::operator==(rhs); }

  auto operator<(const FuncId& rhs) const noexcept -> bool { return m_id < rhs.m_id; }
  auto operator>(const FuncId& rhs) const noexcept -> bool { return m_id > rhs.m_id; }

  [[nodiscard]] auto getNum() const noexcept -> unsigned int { return m_id; }

private:
  unsigned int m_id;

  explicit FuncId(unsigned int id) : m_id{id} {}
};

inline auto operator<<(std::ostream& out, const FuncId& rhs) -> std::ostream& {
  return out << "f-" << rhs.m_id;
}

} // namespace prog::sym
