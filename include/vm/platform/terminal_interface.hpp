#pragma once
#include "vm/platform/interface.hpp"

namespace vm::platform {

class TerminalInterface final : public Interface {
public:
  auto print(const char* data, unsigned int size) -> void override;
};

} // namespace vm::platform
