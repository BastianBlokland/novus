#include "vm/exceptions/eval_stack_overflow.hpp"

namespace vm::exceptions {

auto EvalStackOverflow::what() const noexcept -> const char* { return "Evaluation stack overflow"; }

} // namespace vm::exceptions
