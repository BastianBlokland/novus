#include "vm/exceptions/assert_failed.hpp"

namespace vm::exceptions {

AssertFailed::AssertFailed(std::string msg) : m_msg{std::move(msg)} {}

auto AssertFailed::what() const noexcept -> const char* { return m_msg.data(); }

} // namespace vm::exceptions
