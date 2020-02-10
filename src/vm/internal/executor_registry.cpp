#include "internal/executor_registry.hpp"
#include <thread>

namespace vm::internal {

ExecutorRegistry::ExecutorRegistry() noexcept : m_head{nullptr} {};

auto ExecutorRegistry::registerExecutor(ExecutorHandle* handle) noexcept -> void {
  auto lk = std::lock_guard<std::mutex>{m_mutex};

  assert(handle->m_prev == nullptr);
  assert(handle->m_next == nullptr);
  if (m_head) {
    m_head->m_prev = handle;
    handle->m_next = m_head;
  }
  m_head = handle;
}

auto ExecutorRegistry::unregisterExecutor(ExecutorHandle* handle) noexcept -> void {
  auto lk = std::lock_guard<std::mutex>{m_mutex};

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
  /* Keep looping over all executors and requesting abort until all of them have aborted. */
  while (true) {
    bool done = true;
    {
      auto lk    = std::lock_guard<std::mutex>{m_mutex};
      auto* exec = m_head;
      while (exec) {
        done &= exec->requestAbort();
        exec = exec->m_next;
      }
    }
    if (done) {
      break;
    }
    std::this_thread::yield();
  }
}

auto ExecutorRegistry::pauseExecutors() noexcept -> void {
  /* Keep looping over all executors and requesting pause until all of them have paused. */
  while (true) {
    bool done = true;
    {
      auto lk    = std::lock_guard<std::mutex>{m_mutex};
      auto* exec = m_head;
      while (exec) {
        done &= exec->requestPause();
        exec = exec->m_next;
      }
    }
    if (done) {
      break;
    }
    std::this_thread::yield();
  }
}

auto ExecutorRegistry::resumeExecutors() noexcept -> void {
  /* Unset the pause flag on all executors. */
  auto lk    = std::lock_guard<std::mutex>{m_mutex};
  auto* exec = m_head;
  while (exec) {
    exec->resume();
    exec = exec->m_next;
  }
}

} // namespace vm::internal
