#include "lex/token.hpp"
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

} // namespace lex
