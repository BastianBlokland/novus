#include "vm/exceptions/call_stack_overflow.hpp"

namespace vm::exceptions {

auto CallStackOverflow::what() const noexcept -> const char* { return "Call stack overflow"; }

} // namespace vm::exceptions
