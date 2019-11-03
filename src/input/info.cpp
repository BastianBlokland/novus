#include "input/info.hpp"
#include <algorithm>

namespace input {

Info::Info() : m_end{} {}

auto Info::markLineEnd(const unsigned int pos) -> void {
  assert(m_lines.empty() || pos > m_lines.back());
  m_lines.push_back(pos);
}

auto Info::markFileEnd(const unsigned int pos) -> void { m_end = pos; }

auto Info::getLineCount() const noexcept -> unsigned int { return m_lines.size(); }

auto Info::getCharCount() const noexcept -> unsigned int { return m_end; }

auto Info::getTextPos(const unsigned int pos) const noexcept -> TextPos {
  const auto begin = m_lines.begin();
  const auto lower = std::lower_bound(begin, m_lines.end(), pos);
  const auto line  = static_cast<unsigned int>(lower - begin) + 1;
  const auto col   = lower == begin ? (pos + 1) : (pos - *(lower - 1));
  return TextPos{line, col};
}

} // namespace input
