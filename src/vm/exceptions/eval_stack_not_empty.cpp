#include "vm/exceptions/eval_stack_not_empty.hpp"

namespace vm::exceptions {

auto EvalStackNotEmpty::what() const noexcept -> const char* {
  return "Evaluation stack not empty when scope returned";
}

} // namespace vm::exceptions
