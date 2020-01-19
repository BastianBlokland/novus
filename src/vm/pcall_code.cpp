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
  }
  return out;
}

} // namespace vm
