#pragma once

namespace vm::platform {

class Interface {
public:
  virtual auto print(const char* data, unsigned int size) -> void = 0;
};

} // namespace vm::platform
