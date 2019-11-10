#pragma once
#include <iostream>

namespace prog::sym {

class ConstId final {
  friend class ConstDeclTable;
  friend auto operator<<(std::ostream& out, const ConstId& rhs) -> std::ostream&;

public:
  auto operator==(const ConstId& rhs) const noexcept -> bool;
  auto operator!=(const ConstId& rhs) const noexcept -> bool;

private:
  unsigned int m_id;

  explicit ConstId(unsigned int id);
};

auto operator<<(std::ostream& out, const ConstId& rhs) -> std::ostream&;

} // namespace prog::sym
