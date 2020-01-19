#include "vm/platform/terminal_interface.hpp"
#include <cstdio>

namespace vm::platform {

// NOLINTNEXTLINE: Used as a template typename where signature matters.
auto TerminalInterface::print(const char* data, unsigned int size) -> void {
  std::fwrite(data, sizeof(char), size, stdout);
}

} // namespace vm::platform
