#pragma once
#include <iostream>

namespace prog::sym {

class TypeId final {
  friend class TypeDeclTable;
  friend auto operator<<(std::ostream& out, const TypeId& rhs) -> std::ostream&;

public:
  auto operator==(const TypeId& rhs) const noexcept -> bool;
  auto operator!=(const TypeId& rhs) const noexcept -> bool;

private:
  unsigned int m_id;

  explicit TypeId(unsigned int id);
};

auto operator<<(std::ostream& out, const TypeId& rhs) -> std::ostream&;

} // namespace prog::sym