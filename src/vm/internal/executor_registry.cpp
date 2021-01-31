#include "internal/executor_registry.hpp"
#include <thread>

namespace vm::internal {

ExecutorRegistry::ExecutorRegistry() noexcept : m_head{nullptr}, m_state{RegistryState::Running} {};

auto ExecutorRegistry::registerExecutor(ExecutorHandle* handle) noexcept -> void {
  assert(m_state.load(std::memory_order_acquire) == RegistryState::Running);
  assert(handle->m_prev == nullptr);
  assert(handle->m_next == nullptr);

  auto lk = std::lock_guard<std::mutex>{m_mutex};

  if (m_head) {
    m_head->m_prev = handle;
    handle->m_next = m_head;
  }
  m_head = handle;
}

auto ExecutorRegistry::unregisterExecutor(ExecutorHandle* handle) noexcept -> void {
  assert(m_state.load(std::memory_order_acquire) == RegistryState::Running);

  auto lk = std::lock_guard<std::mutex>{m_mutex};

  assert(m_head);
  assert(handle == m_head || handle->m_prev);

  if (handle == m_head) {
    m_head = handle->m_next;
  } else {
    handle->m_prev->m_next = handle->m_next;
  }
  if (handle->m_next) {
    handle->m_next->m_prev = handle->m_prev;
  }
}

auto ExecutorRegistry::abortExecutors() noexcept -> void {

  assert(m_state.load(std::memory_order_acquire) == RegistryState::Running);

  // First pause all the executors to they are either trapped or still inside a blocking call (but
  // will trap as soon as the blocking call is done).
  pauseExecutors();

  assert(m_state.load(std::memory_order_acquire) == RegistryState::Paused);

  // Then request all executors to abort.
  // NOTE: After aborting executors its unsafe to access any memory memory belonging to that
  // executor (including the handle).
  // NOTE: Aborted executors will not unregister themselves.
  {
    auto lk    = std::lock_guard<std::mutex>{m_mutex};
    auto* exec = m_head;
    while (exec) {
      exec->requestAbort();
      exec = exec->m_next;
    }
    m_head = nullptr;
  }
  m_state.store(RegistryState::Aborted, std::memory_order_release);
}

auto ExecutorRegistry::pauseExecutors() noexcept -> void {
  assert(m_state.load(std::memory_order_acquire) == RegistryState::Running);

  /* Keep looping over all executors and requesting pause until all of them have paused. */
  while (true) {
    {
      bool done  = true;
      auto lk    = std::lock_guard<std::mutex>{m_mutex};
      auto* exec = m_head;
      while (exec) {
        done &= exec->requestPause();
        exec = exec->m_next;
      }
      if (done) {
        break;
      }
    }
    std::this_thread::yield();
  }

  m_state.store(RegistryState::Paused, std::memory_order_release);
}

auto ExecutorRegistry::resumeExecutors() noexcept -> void {
  assert(m_state.load(std::memory_order_acquire) == RegistryState::Paused);

  m_state.store(RegistryState::Running, std::memory_order_release);

  /* Unset the pause flag on all executors. */
  auto lk    = std::lock_guard<std::mutex>{m_mutex};
  auto* exec = m_head;
  while (exec) {
    exec->resume();
    exec = exec->m_next;
  }
}

} // namespace vm::internal
