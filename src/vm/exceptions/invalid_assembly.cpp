#include "vm/exceptions/invalid_assembly.hpp"

namespace vm::exceptions {

auto InvalidAssembly::what() const noexcept -> const char* { return "Invalid assembly"; }

} // namespace vm::exceptions
