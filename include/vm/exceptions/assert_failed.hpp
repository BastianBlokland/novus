#pragma once
#include <exception>
#include <string>

namespace vm::exceptions {

class AssertFailed final : public std::exception {
public:
  explicit AssertFailed(std::string msg);

  [[nodiscard]] auto what() const noexcept -> const char* override;

private:
  std::string m_msg;
};

} // namespace vm::exceptions
