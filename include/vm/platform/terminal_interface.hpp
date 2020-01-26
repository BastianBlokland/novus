#pragma once
#include <cstdio>
#include <cstdlib>

namespace vm::platform {

class TerminalInterface final {
public:
  // NOLINTNEXTLINE: Cannot be static because it needs to match the interface.
  auto inline print(const char* data, unsigned int size) -> void {
    std::fwrite(data, sizeof(char), size, stdout);
  }

  // NOLINTNEXTLINE: Cannot be static because it needs to match the interface.
  auto inline read() -> char {
    auto res = std::getc(stdin);
    return res > 0 ? static_cast<char>(res) : '\0';
  }

  // NOLINTNEXTLINE: Cannot be static because it needs to match the interface.
  auto inline getEnvVar(const char* name) -> char* { return std::getenv(name); }
};

} // namespace vm::platform
