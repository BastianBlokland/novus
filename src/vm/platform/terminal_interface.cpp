#include "vm/platform/terminal_interface.hpp"
#include <iostream>

namespace vm::platform {

auto TerminalInterface::print(const char* data, unsigned int size) -> void {
  std::cout.write(data, size);
}

} // namespace vm::platform
