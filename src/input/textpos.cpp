#include "input/textpos.hpp"

namespace input {

TextPos::TextPos(unsigned int line, unsigned int col) : m_line{line}, m_col{col} {}

auto TextPos::operator==(const TextPos& rhs) const noexcept -> bool {
  return m_line == rhs.m_line && m_col == rhs.m_col;
}

auto TextPos::operator!=(const TextPos& rhs) const noexcept -> bool {
  return m_line != rhs.m_line || m_col != rhs.m_col;
}

auto TextPos::getLine() const noexcept -> unsigned int { return m_line; }

auto TextPos::getCol() const noexcept -> unsigned int { return m_col; }

auto operator<<(std::ostream& out, const TextPos& rhs) -> std::ostream& {
  out << "ln " << rhs.m_line << ", c " << rhs.m_col;
  return out;
}

} // namespace input
