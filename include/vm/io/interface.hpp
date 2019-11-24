#pragma once

namespace vm::io {

class Interface {
public:
  virtual auto print(const char* data, unsigned int size) -> void = 0;
};

} // namespace vm::io
