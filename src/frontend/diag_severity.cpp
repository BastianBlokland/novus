#include "frontend/diag_severity.hpp"

namespace frontend {

auto operator<<(std::ostream& out, const DiagSeverity& rhs) -> std::ostream& {
  switch (rhs) {
  case DiagSeverity::Warning:
    out << "warning";
    break;
  case DiagSeverity::Error:
    out << "error";
    break;
  }
  return out;
}

} // namespace frontend
