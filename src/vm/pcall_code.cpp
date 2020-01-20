#include "vm/pcall_code.hpp"

namespace vm {

auto operator<<(std::ostream& out, const PCallCode& rhs) -> std::ostream& {
  switch (rhs) {
  case PCallCode::PrintChar:
    out << "print-char";
    break;
  case PCallCode::PrintString:
    out << "print-string";
    break;
  case PCallCode::PrintStringLine:
    out << "print-string-line";
    break;
  case PCallCode::ReadChar:
    out << "read-char";
    break;
  case PCallCode::ReadStringLine:
    out << "read-string-line";
    break;
  case PCallCode::Sleep:
    out << "sleep";
    break;
  case PCallCode::Assert:
    out << "assert";
    break;
  }
  return out;
}

} // namespace vm
