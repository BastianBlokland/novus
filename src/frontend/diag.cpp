#include "frontend/diag.hpp"
#include <utility>

namespace frontend {

Diag::Diag(
    DiagSeverity severity,
    std::string msg,
    std::optional<filesystem::path> sourcePath,
    input::TextPos sourceStart,
    input::TextPos sourceEnd) :
    m_severity{severity},
    m_msg{std::move(msg)},
    m_sourcePath{std::move(sourcePath)},
    m_sourceStart{sourceStart},
    m_sourceEnd{sourceEnd} {};

auto Diag::operator==(const Diag& rhs) const noexcept -> bool {
  return m_severity == rhs.m_severity && m_msg == rhs.m_msg && m_sourcePath == rhs.m_sourcePath &&
      m_sourceStart == rhs.m_sourceStart && m_sourceEnd == rhs.m_sourceEnd;
}

auto Diag::operator!=(const Diag& rhs) const noexcept -> bool { return !Diag::operator==(rhs); }

auto Diag::getSeverity() const noexcept -> DiagSeverity { return m_severity; }

auto Diag::getMsg() const noexcept -> std::string { return m_msg; }

auto Diag::getSourcePath() const noexcept -> std::optional<filesystem::path> {
  return m_sourcePath;
}

auto Diag::getSourceStart() const noexcept -> input::TextPos { return m_sourceStart; }

auto Diag::getSourceEnd() const noexcept -> input::TextPos { return m_sourceEnd; }

auto operator<<(std::ostream& out, const Diag& rhs) -> std::ostream& {
  const auto id        = rhs.m_sourcePath ? rhs.m_sourcePath->string() : "inline";
  const auto startLine = rhs.m_sourceStart.getLine();
  const auto startCol  = rhs.m_sourceStart.getCol();
  const auto endLine   = rhs.m_sourceEnd.getLine();
  const auto endCol = rhs.m_sourceEnd.getCol() + 1; // + 1 because we want to include the last char.

  return out << id << ':' << startLine << ':' << startCol << '-' << endLine << ':' << endCol << ": "
             << rhs.getSeverity() << ": " << rhs.m_msg;
}

// Factories.
auto warning(const Source& src, std::string msg, input::Span span) -> Diag {
  const auto srcStart = src.getTextPos(span.getStart());
  const auto srcEnd   = src.getTextPos(span.getEnd());
  return Diag{DiagSeverity::Warning, std::move(msg), src.getId(), srcStart, srcEnd};
}

auto error(const Source& src, std::string msg, input::Span span) -> Diag {
  const auto srcStart = src.getTextPos(span.getStart());
  const auto srcEnd   = src.getTextPos(span.getEnd());
  return Diag{DiagSeverity::Error, std::move(msg), src.getPath(), srcStart, srcEnd};
}

} // namespace frontend
