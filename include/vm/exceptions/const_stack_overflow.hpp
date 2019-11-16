#pragma once
#include <exception>

namespace vm::exceptions {

class ConstStackOverflow final : public std::exception {
public:
  [[nodiscard]] auto what() const noexcept -> const char* override;
};

} // namespace vm::exceptions
