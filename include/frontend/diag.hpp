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
  [[nodiscard]] auto getSourcePath() const noexcept -> std::optional<filesystem::path>;
  [[nodiscard]] auto getSourceStart() const noexcept -> input::TextPos;
  [[nodiscard]] auto getSourceEnd() const noexcept -> input::TextPos;

private:
  DiagSeverity m_severity;
  std::string m_msg;
  std::optional<filesystem::path> m_sourcePath;
  input::TextPos m_sourceStart;
  input::TextPos m_sourceEnd;

  Diag(
      DiagSeverity severity,
      std::string msg,
      std::optional<filesystem::path> sourcePath,
      input::TextPos sourceStart,
      input::TextPos sourceEnd);
};

auto operator<<(std::ostream& out, const Diag& rhs) -> std::ostream&;

auto warning(const Source& src, std::string msg, input::Span span) -> Diag;
auto error(const Source& src, std::string msg, input::Span span) -> Diag;

} // namespace frontend
