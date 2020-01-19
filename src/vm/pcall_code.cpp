#include "vm/pcall_code.hpp"

namespace vm {

auto operator<<(std::ostream& out, const PCallCode& rhs) -> std::ostream& {
  switch (rhs) {
  case PCallCode::Print:
    out << "print";
    break;
  }
  return out;
}

} // namespace vm
