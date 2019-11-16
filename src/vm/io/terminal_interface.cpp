#include "vm/io/terminal_interface.hpp"
#include <iostream>

namespace vm::io {

auto TerminalInterface::print(std::string text) -> void { std::cout << text << '\n'; }

} // namespace vm::io
