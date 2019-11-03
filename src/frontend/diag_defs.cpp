#include "frontend/diag_defs.hpp"
#include <sstream>

namespace frontend {

auto errParseError(const Source& src, const parse::ErrorNode& n) -> Diag {
  std::ostringstream oss;
  oss << "Parse error: " << n.getMessage();
  return error(src, oss.str(), n.getSpan());
}

auto errUndeclaredType(const Source& src, const std::string& typeName, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "Unkown type: '" << typeName << '\'';
  return error(src, oss.str(), span);
}

auto errFuncNameConflictsWithType(const Source& src, const std::string& funcName, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "Function name '" << funcName << "' conflicts with a type with the same name";
  return error(src, oss.str(), span);
}

auto errFuncNameConflictsWithAction(
    const Source& src, const std::string& funcName, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "Function name '" << funcName << "' conflicts with a action with the same name";
  return error(src, oss.str(), span);
}

auto errDuplicateFuncDeclaration(const Source& src, const std::string& funcName, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "Declaration of function '" << funcName
      << "' conflicts with an existing function with the same name and inputs.";

  return error(src, oss.str(), span);
}

} // namespace frontend
