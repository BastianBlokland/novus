#include "vm/exceptions/stack_not_empty.hpp"

namespace vm::exceptions {

auto StackNotEmpty::what() const noexcept -> const char* {
  return "Stack not empty when scope returned";
}

} // namespace vm::exceptions
