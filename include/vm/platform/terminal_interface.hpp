#pragma once

namespace vm::platform {

class TerminalInterface final {
public:
  auto print(const char* data, unsigned int size) -> void;
  auto read() -> char;
};

} // namespace vm::platform
