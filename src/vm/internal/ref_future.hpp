#pragma once
#include "internal/thread.hpp"
#include "internal/value.hpp"
#include "vm/exec_state.hpp"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>

namespace vm::internal {

class Value;

// A future is a handle to a forked executor that is asynchronously computing (or has computed) a
// value.
class FutureRef final : public Ref {
  friend class RefAllocator;

public:
  FutureRef(const FutureRef& rhs) = delete;
  FutureRef(FutureRef&& rhs)      = delete;
  ~FutureRef() noexcept {
    {
      auto lk = std::lock_guard<std::mutex>{m_mutex};
      if (m_state == ExecState::Running) {
        m_state = ExecState::Aborted;
      }
      m_condVar.notify_all();
    }

    // Wait until all waiters have received the abort message.
    while (m_waitersCount.load(std::memory_order_acquire)) {
      threadPause();
    }
  }

  auto operator=(const FutureRef& rhs) -> FutureRef& = delete;
  auto operator=(FutureRef&& rhs) -> FutureRef& = delete;

  [[nodiscard]] constexpr static auto getKind() { return RefKind::Future; }

  // Block until the value has been computed (or the executor failed).
  [[nodiscard]] inline auto block() noexcept -> ExecState {
    m_waitersCount.fetch_add(1, std::memory_order_release);

    {
      auto lk = std::unique_lock<std::mutex>{m_mutex};
      m_condVar.wait(lk, [this] { return m_state != ExecState::Running; });
    }

    m_waitersCount.fetch_sub(1, std::memory_order_release);
    return m_state;
  }

  // Block until the value has been computed or a timeout occurs.
  [[nodiscard]] inline auto waitNano(int64_t timeout) noexcept -> bool {
    m_waitersCount.fetch_add(1, std::memory_order_release);

    bool res;
    {
      auto lk = std::unique_lock<std::mutex>{m_mutex};
      res     = m_condVar.wait_for(
          lk, std::chrono::nanoseconds(timeout), [this] { return m_state != ExecState::Running; });
    }

    m_waitersCount.fetch_sub(1, std::memory_order_release);
    return res;
  }

  // Check the state of the executor that is computing the value.
  [[nodiscard]] inline auto poll() noexcept -> ExecState {
    auto lk = std::lock_guard<std::mutex>{m_mutex};
    return m_state;
  }

  // Check if the executor has started computing the value.
  [[nodiscard]] inline auto getStarted() noexcept -> std::atomic_bool& { return m_started; }

  [[nodiscard]] inline auto getResult() noexcept -> Value { return m_result; }

  inline auto setResult(Value result) noexcept { m_result = result; }

  inline auto setState(ExecState state) noexcept {
    auto lk = std::lock_guard<std::mutex>{m_mutex};
    m_state = state;
    m_condVar.notify_all();
  }

private:
  std::atomic_bool m_started;
  ExecState m_state;
  std::mutex m_mutex;
  std::condition_variable m_condVar;
  std::atomic<uint32_t> m_waitersCount;
  Value m_result;

  inline explicit FutureRef() noexcept :
      Ref(getKind()),
      m_started{false},
      m_state{ExecState::Running},
      m_mutex{},
      m_condVar{},
      m_waitersCount{0},
      m_result{} {}
};

inline auto getFutureRef(const Value& val) noexcept { return val.getDowncastRef<FutureRef>(); }

} // namespace vm::internal
