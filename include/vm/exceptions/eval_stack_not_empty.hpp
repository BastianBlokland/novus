#pragma once
#include <exception>

namespace vm::exceptions {

class EvalStackNotEmpty final : public std::exception {
public:
  [[nodiscard]] auto what() const noexcept -> const char* override;
};

} // namespace vm::exceptions
