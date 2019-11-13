#pragma once
#include <exception>

namespace vm::exceptions {

class StackOverflow final : public std::exception {};

} // namespace vm::exceptions
