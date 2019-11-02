#include "frontend/diag.hpp"
#include <utility>

namespace frontend {

Diag::Diag(
    DiagSeverity severity,
    std::string msg,
    std::string sourceId,
    input::Span sourceSpan,
    input::TextPos sourcePos) :
    m_severity{severity},
    m_msg{std::move(msg)},
    m_sourceId{std::move(sourceId)},
    m_sourceSpan{sourceSpan},
    m_sourcePos{sourcePos} {};

auto Diag::operator==(const Diag& rhs) const noexcept -> bool {
  return m_severity == rhs.m_severity && m_sourceId == rhs.m_sourceId &&
      m_sourceSpan == rhs.m_sourceSpan && m_sourcePos == rhs.m_sourcePos;
}

auto Diag::operator!=(const Diag& rhs) const noexcept -> bool { return !Diag::operator==(rhs); }

auto Diag::getSeverity() const noexcept -> DiagSeverity { return m_severity; }

auto Diag::getMsg() const noexcept -> std::string { return m_msg; }

auto Diag::getSourceId() const noexcept -> std::string { return m_sourceId; }

auto Diag::getSourceSpan() const noexcept -> input::Span { return m_sourceSpan; }

auto Diag::getSourcePos() const noexcept -> input::TextPos { return m_sourcePos; }

auto operator<<(std::ostream& out, const Diag& rhs) -> std::ostream& {
  return out << rhs.getSeverity() << " (" << rhs.m_sourceId << " " << rhs.getSourcePos() << ") "
             << rhs.m_msg;
}

auto warning(const Source& src, std::string msg, input::Span span) -> Diag {
  const auto srcPos = src.getTextPos(span.getStart());
  return Diag{DiagSeverity::Warning, std::move(msg), src.getId(), span, srcPos};
}

auto error(const Source& src, std::string msg, input::Span span) -> Diag {
  const auto srcPos = src.getTextPos(span.getStart());
  return Diag{DiagSeverity::Error, std::move(msg), src.getId(), span, srcPos};
}

} // namespace frontend
