#include "frontend/diag.hpp"
#include "frontend/source_table.hpp"
#include <iostream>
#include <utility>

namespace frontend {

Diag::Diag(DiagSeverity severity, std::string msg, prog::sym::SourceId src) :
    m_severity{severity}, m_msg{std::move(msg)}, m_src{src} {};
auto Diag::operator==(const Diag& rhs) const noexcept -> bool {
  return m_severity == rhs.m_severity && m_msg == rhs.m_msg;
}

auto Diag::operator!=(const Diag& rhs) const noexcept -> bool { return !Diag::operator==(rhs); }
auto Diag::getSeverity() const noexcept -> DiagSeverity { return m_severity; }

auto Diag::getMsg() const noexcept -> std::string { return m_msg; }

auto Diag::getSrc() const noexcept -> prog::sym::SourceId { return m_src; }

auto Diag::print(std::ostream& stream, const SourceTable& sourceTable) const -> void {
  const auto& sourceInfo = sourceTable[m_src];

  const auto id        = sourceInfo.getPath() ? sourceInfo.getPath()->string() : "inline";
  const auto startLine = sourceInfo.getStart().getLine();
  const auto startCol  = sourceInfo.getStart().getCol();
  const auto endLine   = sourceInfo.getEnd().getLine();
  const auto endCol    = sourceInfo.getEnd().getCol() + 1; // + 1 to include the last char.

  stream << id << ':' << startLine << ':' << startCol << '-' << endLine << ':' << endCol << ": "
         << m_severity << ": " << m_msg;
}

auto operator<<(std::ostream& out, Diag diag) -> std::ostream& {
  return out << diag.getSeverity() << ": " << diag.getMsg();
}

// Factories.
auto warning(std::string msg, prog::sym::SourceId src) -> Diag {
  return Diag{DiagSeverity::Warning, std::move(msg), src};
}

auto error(std::string msg, prog::sym::SourceId src) -> Diag {
  return Diag{DiagSeverity::Error, std::move(msg), src};
}

} // namespace frontend
