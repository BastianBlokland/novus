#pragma once
#include <iostream>

namespace prog::sym {

class ActionId final {
  friend class ActionDeclTable;
  friend auto operator<<(std::ostream& out, const ActionId& rhs) -> std::ostream&;

public:
  auto operator==(const ActionId& rhs) const noexcept -> bool;
  auto operator!=(const ActionId& rhs) const noexcept -> bool;

private:
  unsigned int m_id;

  explicit ActionId(unsigned int id);
};

auto operator<<(std::ostream& out, const ActionId& rhs) -> std::ostream&;

} // namespace prog::sym
