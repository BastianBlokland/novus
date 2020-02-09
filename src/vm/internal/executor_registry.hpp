#pragma once
#include "internal/executor_handle.hpp"
#include "internal/stack.hpp"
#include <mutex>

namespace vm::internal {

class ExecutorRegistry final {
public:
  ExecutorRegistry() noexcept;
  ExecutorRegistry(const ExecutorRegistry& rhs) = delete;
  ExecutorRegistry(ExecutorRegistry&& rhs)      = delete;
  ~ExecutorRegistry() noexcept                  = default;

  auto operator=(const ExecutorRegistry& rhs) -> ExecutorRegistry& = delete;
  auto operator=(ExecutorRegistry&& rhs) -> ExecutorRegistry& = delete;

  auto registerExecutor(ExecutorHandle* handle) noexcept -> void;
  auto unregisterExecutor(ExecutorHandle* handle) noexcept -> void;

  auto abortExecutors() noexcept -> void;
  auto pauseExecutors() noexcept -> void;
  auto resumeExecutors() noexcept -> void;

private:
  std::mutex m_mutex;
  ExecutorHandle* m_head;
};

} // namespace vm::internal
