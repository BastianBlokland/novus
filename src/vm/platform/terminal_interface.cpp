#include "vm/platform/terminal_interface.hpp"
#include <cstdio>

namespace vm::platform {

// NOLINTNEXTLINE: Cannot make static because its used as a template.
auto TerminalInterface::print(const char* data, unsigned int size) -> void {
  std::fwrite(data, sizeof(char), size, stdout);
}

// NOLINTNEXTLINE: Cannot make static because its used as a template.
auto TerminalInterface::read() -> char {
  auto res = std::getc(stdin);
  return res > 0 ? static_cast<char>(res) : '\0';
}

} // namespace vm::platform
