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

} // namespace lex
