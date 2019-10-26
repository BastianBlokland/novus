#pragma once
#include <iostream>

namespace prog::sym {

class FuncId final {
  friend class FuncDeclTable;
  friend class FuncDefTable;
  friend auto operator<<(std::ostream& out, const FuncId& rhs) -> std::ostream&;

public:
  auto operator==(const FuncId& rhs) const noexcept -> bool;
  auto operator!=(const FuncId& rhs) const noexcept -> bool;

private:
  unsigned int m_id;

  explicit FuncId(unsigned int id);
};

auto operator<<(std::ostream& out, const FuncId& rhs) -> std::ostream&;

} // namespace prog::sym
