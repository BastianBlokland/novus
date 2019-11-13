#pragma once
#include <exception>

namespace vm::exceptions {

class InvalidProgram final : public std::exception {};

} // namespace vm::exceptions
