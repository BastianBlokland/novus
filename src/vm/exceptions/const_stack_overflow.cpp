#include "vm/exceptions/const_stack_overflow.hpp"

namespace vm::exceptions {

auto ConstStackOverflow::what() const noexcept -> const char* { return "Constants stack overflow"; }

} // namespace vm::exceptions
