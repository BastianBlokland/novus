#pragma once
#include "frontend/diag_severity.hpp"
#include "frontend/source.hpp"
#include "input/span.hpp"
#include "input/textpos.hpp"
#include <string>

namespace frontend {

class Diag final {
  friend auto warning(const Source& src, std::string msg, input::Span span) -> Diag;
  friend auto error(const Source& src, std::string msg, input::Span span) -> Diag;
  friend auto operator<<(std::ostream& out, const Diag& rhs) -> std::ostream&;

public:
  Diag() = delete;

  auto operator==(const Diag& rhs) const noexcept -> bool;
  auto operator!=(const Diag& rhs) const noexcept -> bool;

  [[nodiscard]] auto getSeverity() const noexcept -> DiagSeverity;
  [[nodiscard]] auto getMsg() const noexcept -> std::string;
  [[nodiscard]] auto getSourceId() const noexcept -> std::string;
  [[nodiscard]] auto getSourceSpan() const noexcept -> input::Span;
  [[nodiscard]] auto getSourcePos() const noexcept -> input::TextPos;

private:
  DiagSeverity m_severity;
  std::string m_msg;
  std::string m_sourceId;
  input::Span m_sourceSpan;
  input::TextPos m_sourcePos;

  Diag(
      DiagSeverity severity,
      std::string msg,
      std::string sourceId,
      input::Span sourceSpan,
      input::TextPos sourcePos);
};

auto operator<<(std::ostream& out, const Diag& rhs) -> std::ostream&;

auto warning(const Source& src, std::string msg, input::Span span) -> Diag;
auto error(const Source& src, std::string msg, input::Span span) -> Diag;

} // namespace frontend
