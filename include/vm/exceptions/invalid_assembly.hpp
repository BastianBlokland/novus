#pragma once
#include <exception>

namespace vm::exceptions {

class InvalidAssembly final : public std::exception {};

} // namespace vm::exceptions
