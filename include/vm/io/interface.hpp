#pragma once
#include <string>

namespace vm::io {

class Interface {
public:
  virtual auto print(std::string text) -> void = 0;
};

} // namespace vm::io
