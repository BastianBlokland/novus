#pragma once
#include "vm/io/interface.hpp"

namespace vm::io {

class TerminalInterface final : public Interface {
public:
  auto print(const char* data, unsigned int size) -> void override;
};

} // namespace vm::io
