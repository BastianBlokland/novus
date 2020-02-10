#pragma once
#include "internal/stack.hpp"
#include "vm/exec_state.hpp"
#include <atomic>
#include <thread>

namespace vm::internal {

class ExecutorRegistry;

class ExecutorHandle final {
  friend ExecutorRegistry;

  enum class RequestType : int {
    None  = 0,
    Abort = 1,
    Pause = 2,
  };

public:
  explicit ExecutorHandle() noexcept :
      m_state{ExecState::Running}, m_request{RequestType::None}, m_prev{nullptr}, m_next{nullptr} {}
  ExecutorHandle(const ExecutorHandle& rhs) = delete;
  ExecutorHandle(ExecutorHandle&& rhs)      = delete;
  ~ExecutorHandle() noexcept                = default;

  auto operator=(const ExecutorHandle& rhs) -> ExecutorHandle& = delete;
  auto operator=(ExecutorHandle&& rhs) -> ExecutorHandle& = delete;

  [[nodiscard]] inline auto
  getState(std::memory_order memOrder = std::memory_order_acquire) noexcept -> ExecState {
    return m_state.load(memOrder);
  }

  inline auto setState(ExecState state) noexcept -> void {
    m_state.store(state, std::memory_order_release);
  }

  inline auto trap() noexcept -> bool {
    auto req = m_request.load(std::memory_order_acquire);
    switch (req) {
    case RequestType::Abort:
    Abort:
      m_state.store(ExecState::Aborted, std::memory_order_release);
      return true;
    case RequestType::Pause:
      m_state.store(ExecState::Paused, std::memory_order_release);
      while (req = m_request.load(std::memory_order_acquire), req == RequestType::Pause) {
        std::this_thread::yield();
      }
      if (req == RequestType::Abort) {
        goto Abort;
      }
      m_state.store(ExecState::Running, std::memory_order_release);
      [[fallthrough]];
    case RequestType::None:
      return false;
    }
  }

  inline auto requestAbort() noexcept -> bool {
    m_request.store(RequestType::Abort, std::memory_order_release);
    return m_state.load(std::memory_order_acquire) != ExecState::Running;
  }

  inline auto requestPause() noexcept -> bool {
    // Set request to 'Pause' in case its currently 'None', reason is we want to leave it alone when
    // its currently set to 'Abort' to avoid resurrecting aborted executors.
    auto currentReq = m_request.load(std::memory_order_relaxed);
    if (currentReq == RequestType::None) {
      m_request.compare_exchange_strong(
          currentReq, RequestType::Pause, std::memory_order_release, std::memory_order_relaxed);
    }
    return m_state.load(std::memory_order_acquire) != ExecState::Running;
  }

  inline auto resume() noexcept -> void {
    auto currentReq = m_request.load(std::memory_order_relaxed);
    if (currentReq == RequestType::Pause) {
      m_request.compare_exchange_strong(
          currentReq, RequestType::None, std::memory_order_release, std::memory_order_relaxed);
    }
  }

private:
  std::atomic<ExecState> m_state;
  std::atomic<RequestType> m_request;

  ExecutorHandle* m_prev;
  ExecutorHandle* m_next;
};

} // namespace vm::internal
