#include "prog/sym/input.hpp"

namespace prog::sym {

Input::Input(std::initializer_list<TypeId> list) : m_val{list} {}

Input::Input(std::vector<TypeId> val) : m_val{std::move(val)} {}

auto Input::operator[](unsigned int i) const -> TypeId {
  assert(i < this->m_val.size());
  return m_val[i];
}

auto Input::operator==(const Input& rhs) const noexcept -> bool { return m_val == rhs.m_val; }

auto Input::operator!=(const Input& rhs) const noexcept -> bool { return !Input::operator==(rhs); }

auto Input::getCount() const -> unsigned int { return m_val.size(); }

auto Input::begin() const -> iterator { return m_val.begin(); }

auto Input::end() const -> iterator { return m_val.end(); }

auto operator<<(std::ostream& out, const Input& rhs) -> std::ostream& {
  for (auto i = 0U; i < rhs.m_val.size(); ++i) {
    if (i != 0) {
      out << ", ";
    }
    out << rhs.m_val[i];
  }
  return out;
}

} // namespace prog::sym
