#pragma once

namespace lex {

class SourceSpan final {
public:
  SourceSpan() = delete;
  SourceSpan(const int pos) : SourceSpan(pos, pos){};
  SourceSpan(const int start, const int end);

  auto getStart() const noexcept { return m_start; }

  auto getEnd() const noexcept { return m_end; }

private:
  int m_start, m_end;
};

} // namespace lex
