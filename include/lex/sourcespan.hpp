#pragma once

namespace lex {

class SourceSpan final {
public:
  SourceSpan() = delete;
  SourceSpan(const int start, const int end);

  auto getStart() const -> int;
  auto getEnd() const -> int;

private:
  const int _start, _end;
};

} // namespace lex
