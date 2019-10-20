#include "lex/source_span.hpp"
#include <stdexcept>

namespace lex {

SourceSpan::SourceSpan(const int start, const int end) : m_start{start}, m_end{end} {
  if (start < 0) {
    throw std::invalid_argument("Start cannot be negative");
  }
  if (end < start) {
    throw std::invalid_argument("End cannot be smaller then Start");
  }
}

auto SourceSpan::operator==(const SourceSpan& rhs) const noexcept -> bool {
  return m_start == rhs.m_start && m_end == rhs.m_end;
}

auto SourceSpan::operator!=(const SourceSpan& rhs) const noexcept -> bool {
  return m_start != rhs.m_start || m_end != rhs.m_end;
}

auto SourceSpan::operator<(const SourceSpan& rhs) const noexcept -> bool {
  return m_end < rhs.m_start;
}

auto SourceSpan::operator>(const SourceSpan& rhs) const noexcept -> bool {
  return m_start > rhs.m_end;
}

auto SourceSpan::operator<(const int& rhs) const noexcept -> bool { return m_end < rhs; }

auto SourceSpan::operator>(const int& rhs) const noexcept -> bool { return m_start > rhs; }

auto SourceSpan::combine(SourceSpan a, SourceSpan b) -> SourceSpan {
  return SourceSpan{std::min(a.m_start, b.m_start), std::max(a.m_end, b.m_end)};
}

auto operator<<(std::ostream& out, const SourceSpan& rhs) -> std::ostream& {
  out << rhs.getStart() << '-' << rhs.getEnd();
  return out;
}

} // namespace lex
