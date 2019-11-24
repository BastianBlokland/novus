#include "vm/io/terminal_interface.hpp"
#include <iostream>

namespace vm::io {

auto TerminalInterface::print(const char* data, unsigned int size) -> void {
  std::cout.write(data, size);
}

} // namespace vm::io
