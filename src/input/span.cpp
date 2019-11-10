#include "input/span.hpp"
#include <stdexcept>

namespace input {

Span::Span(const int start, const int end) :
    m_start{static_cast<unsigned int>(start)}, m_end{static_cast<unsigned int>(end)} {
  if (start < 0) {
    throw std::invalid_argument{"Start cannot be negative"};
  }
  if (end < start) {
    throw std::invalid_argument{"End cannot be smaller then Start"};
  }
}

Span::Span(const unsigned int start, const unsigned int end) : m_start{start}, m_end{end} {
  if (end < start) {
    throw std::invalid_argument{"End cannot be smaller then Start"};
  }
}

auto Span::operator==(const Span& rhs) const noexcept -> bool {
  return m_start == rhs.m_start && m_end == rhs.m_end;
}

auto Span::operator!=(const Span& rhs) const noexcept -> bool {
  return m_start != rhs.m_start || m_end != rhs.m_end;
}

auto Span::operator<(const Span& rhs) const noexcept -> bool { return m_end < rhs.m_start; }

auto Span::operator>(const Span& rhs) const noexcept -> bool { return m_start > rhs.m_end; }

auto Span::operator<(const unsigned int& rhs) const noexcept -> bool { return m_end < rhs; }

auto Span::operator>(const unsigned int& rhs) const noexcept -> bool { return m_start > rhs; }

auto Span::combine(Span a, Span b) -> Span {
  return Span{std::min(a.m_start, b.m_start), std::max(a.m_end, b.m_end)};
}

auto operator<<(std::ostream& out, const Span& rhs) -> std::ostream& {
  out << rhs.getStart() << '-' << rhs.getEnd();
  return out;
}

} // namespace input
