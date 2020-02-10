#pragma once
#include "internal/value.hpp"
#include "vm/exec_state.hpp"
#include <atomic>
#include <condition_variable>
#include <mutex>

namespace vm::internal {

class Value;

class FutureRef final : public Ref {
  friend class Allocator;

public:
  FutureRef(const FutureRef& rhs) = delete;
  FutureRef(FutureRef&& rhs)      = delete;
  ~FutureRef() noexcept override  = default;

  auto operator=(const FutureRef& rhs) -> FutureRef& = delete;
  auto operator=(FutureRef&& rhs) -> FutureRef& = delete;

  [[nodiscard]] inline auto wait() noexcept -> ExecState {
    auto lk = std::unique_lock<std::mutex>{m_mutex};
    m_condVar.wait(lk, [this] { return m_state != ExecState::Running; });
    return m_state;
  }

  [[nodiscard]] inline auto poll() noexcept -> ExecState {
    auto lk = std::lock_guard<std::mutex>{m_mutex};
    return m_state;
  }

  [[nodiscard]] inline auto getStarted() noexcept -> std::atomic_bool& { return m_started; }
  [[nodiscard]] inline auto getResult() noexcept -> Value { return m_result; }
  [[nodiscard]] inline auto getMutex() noexcept -> std::mutex& { return m_mutex; }
  [[nodiscard]] inline auto getCondVar() noexcept -> std::condition_variable& { return m_condVar; }

  inline auto setResult(Value result) noexcept { m_result = result; }
  inline auto setState(ExecState state) noexcept { m_state = state; }

private:
  std::atomic_bool m_started;
  ExecState m_state;
  std::mutex m_mutex;
  std::condition_variable m_condVar;
  Value m_result;

  inline explicit FutureRef() noexcept :
      Ref(RefKind::Future),
      m_started{false},
      m_state{ExecState::Running},
      m_mutex{},
      m_condVar{},
      m_result{} {}
};

inline auto getFutureRef(const Value& val) noexcept {
  auto* ref = val.getRef();
  assert(ref->getKind() == RefKind::Future);
  return static_cast<FutureRef*>(ref); // NOLINT: Down-cast.
}

} // namespace vm::internal
