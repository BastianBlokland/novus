#include "lex/token.hpp"
#include <stdexcept>

namespace lex {

SourceSpan::SourceSpan(const int start, const int end)
    : _start{start}, _end{end} {

  if (start < 0) {
    throw std::invalid_argument("Start cannot be negative");
  }
  if (end < start) {
    throw std::invalid_argument("End cannot be smaller then Start");
  }
}

auto SourceSpan::getStart() const -> int { return _start; }

auto SourceSpan::getEnd() const -> int { return _end; }

} // namespace lex
