#include "vm/exceptions/div_by_zero.hpp"

namespace vm::exceptions {

auto DivByZero::what() const noexcept -> const char* { return "Division by zero"; }

} // namespace vm::exceptions
