#include "vm/exceptions/stack_overflow.hpp"

namespace vm::exceptions {

auto StackOverflow::what() const noexcept -> const char* { return "Stack Overflow"; }

} // namespace vm::exceptions
