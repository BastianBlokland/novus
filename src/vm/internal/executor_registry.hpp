#pragma once
#include "internal/executor_handle.hpp"
#include "internal/stack.hpp"
#include <atomic>
#include <mutex>

namespace vm::internal {

// Registry that keeps track of all executors.
class ExecutorRegistry final {
public:
  ExecutorRegistry() noexcept;
  ExecutorRegistry(const ExecutorRegistry& rhs) = delete;
  ExecutorRegistry(ExecutorRegistry&& rhs)      = delete;
  ~ExecutorRegistry() noexcept                  = default;

  auto operator=(const ExecutorRegistry& rhs) -> ExecutorRegistry& = delete;
  auto operator=(ExecutorRegistry&& rhs) -> ExecutorRegistry& = delete;

  [[nodiscard]] auto getHeadExecutor() noexcept -> ExecutorHandle* { return m_head; }

  [[nodiscard]] auto isRunning() noexcept {
    return m_state.load(std::memory_order_acquire) == RegistryState::Running;
  }

  [[nodiscard]] auto isPaused() noexcept {
    return m_state.load(std::memory_order_acquire) == RegistryState::Paused;
  }

  [[nodiscard]] auto isAborted() noexcept {
    return m_state.load(std::memory_order_acquire) == RegistryState::Aborted;
  }

  auto registerExecutor(ExecutorHandle* handle) noexcept -> void;
  auto unregisterExecutor(ExecutorHandle* handle) noexcept -> void;

  auto abortExecutors() noexcept -> void;
  auto pauseExecutors() noexcept -> void;
  auto resumeExecutors() noexcept -> void;

private:
  enum class RegistryState : int {
    Running = 0,
    Paused  = 1,
    Aborted = 2,
  };

  std::mutex m_mutex;
  ExecutorHandle* m_head;
  std::atomic<RegistryState> m_state;
};

} // namespace vm::internal
