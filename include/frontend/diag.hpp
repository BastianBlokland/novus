#pragma once
#include "frontend/diag_severity.hpp"
#include "prog/sym/source_id.hpp"
#include <string>

namespace frontend {

class SourceTable;

class Diag final {
  friend auto warning(std::string msg, prog::sym::SourceId src) -> Diag;
  friend auto error(std::string msg, prog::sym::SourceId src) -> Diag;

public:
  Diag() = delete;

  auto operator==(const Diag& rhs) const noexcept -> bool;
  auto operator!=(const Diag& rhs) const noexcept -> bool;

  [[nodiscard]] auto getSeverity() const noexcept -> DiagSeverity;
  [[nodiscard]] auto getMsg() const noexcept -> std::string;
  [[nodiscard]] auto getSrc() const noexcept -> prog::sym::SourceId;

  auto print(std::ostream& stream, const SourceTable& sourceTable) const -> void;

private:
  DiagSeverity m_severity;
  std::string m_msg;
  prog::sym::SourceId m_src;

  Diag(DiagSeverity severity, std::string msg, prog::sym::SourceId src);
};

auto warning(std::string msg, prog::sym::SourceId src) -> Diag;
auto error(std::string msg, prog::sym::SourceId src) -> Diag;

} // namespace frontend
