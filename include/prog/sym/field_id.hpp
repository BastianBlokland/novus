#pragma once
#include <iostream>

namespace prog::sym {

class FieldId final {
  friend class FieldDeclTable;
  friend auto operator<<(std::ostream& out, const FieldId& rhs) -> std::ostream&;

public:
  auto operator==(const FieldId& rhs) const noexcept -> bool;
  auto operator!=(const FieldId& rhs) const noexcept -> bool;

  [[nodiscard]] auto getNum() const noexcept -> unsigned int;

private:
  unsigned int m_id;

  explicit FieldId(unsigned int id);
};

auto operator<<(std::ostream& out, const FieldId& rhs) -> std::ostream&;

} // namespace prog::sym
