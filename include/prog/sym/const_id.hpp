#pragma once
#include <iostream>

namespace prog::sym {

// Identifier for a constant, wrapper around an unsigned int.
class ConstId final {
  friend class ConstDeclTable;
  friend class ConstIdHasher;
  friend auto operator<<(std::ostream& out, const ConstId& rhs) -> std::ostream&;

public:
  auto operator==(const ConstId& rhs) const noexcept -> bool;
  auto operator!=(const ConstId& rhs) const noexcept -> bool;

  auto operator<(const ConstId& rhs) const noexcept -> bool;
  auto operator>(const ConstId& rhs) const noexcept -> bool;

  [[nodiscard]] auto getNum() const noexcept -> unsigned int;

private:
  unsigned int m_id;

  explicit ConstId(unsigned int id);
};

auto operator<<(std::ostream& out, const ConstId& rhs) -> std::ostream&;

} // namespace prog::sym
