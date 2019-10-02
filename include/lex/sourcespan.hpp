#pragma once

namespace lex {

class SourceSpan final {
public:
  SourceSpan() = delete;
  SourceSpan(const int pos) : SourceSpan(pos, pos){};
  SourceSpan(const int start, const int end);

  auto getStart() const noexcept -> int { return _start; }

  auto getEnd() const noexcept -> int { return _end; }

private:
  int _start, _end;
};

} // namespace lex
