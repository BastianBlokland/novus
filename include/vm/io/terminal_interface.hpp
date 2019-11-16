#pragma once
#include "vm/io/interface.hpp"

namespace vm::io {

class TerminalInterface final : public Interface {
public:
  auto print(std::string text) -> void override;
};

} // namespace vm::io
