#pragma once
#include "internal/intrinsics.hpp"
#include "internal/stack.hpp"
#include "vm/exec_state.hpp"
#include <atomic>
#include <immintrin.h>
#include <thread>

namespace vm::internal {

class ExecutorRegistry;

// Handle to an executor, an executor is a single thread that is executing novus assembly.
// Each executor has its own virtual stack (that is stored on the hardware stack) and a simple
// api to interact with the executor (to request it to pause for example).
//
// Executors have a 'prev' and a 'next' to form a doubly linked list of executors.
//
class ExecutorHandle final {
  friend ExecutorRegistry;

public:
  explicit ExecutorHandle(BasicStack* stack) noexcept :
      m_stack{stack},
      m_state{ExecState::Running},
      m_request{RequestType::None},
      m_prev{nullptr},
      m_next{nullptr} {}
  ExecutorHandle(const ExecutorHandle& rhs) = delete;
  ExecutorHandle(ExecutorHandle&& rhs)      = delete;
  ~ExecutorHandle() noexcept                = default;

  auto operator=(const ExecutorHandle& rhs) -> ExecutorHandle& = delete;
  auto operator=(ExecutorHandle&& rhs) -> ExecutorHandle& = delete;

  [[nodiscard]] inline auto getStack() noexcept -> BasicStack* { return m_stack; }
  [[nodiscard]] inline auto getNext() noexcept -> ExecutorHandle* { return m_next; }

  [[nodiscard]] inline auto
  getState(std::memory_order memOrder = std::memory_order_acquire) noexcept -> ExecState {
    return m_state.load(memOrder);
  }

  inline auto setState(ExecState state) noexcept -> void {
    m_state.store(state, std::memory_order_release);
  }

  // Called by the executor at safe-points in the execution, safe meaning that all data is written
  // back to the stack and the current state is safe to be observed.
  //
  // If no pause request has been placed than trap returns immediately, if pause was requested then
  // trap blocks until its un-paused again.
  //
  [[nodiscard]] inline auto trap() noexcept -> bool {
  TrapBegin:

    auto req = m_request.load(std::memory_order_acquire);
    switch (req) {
    case RequestType::Abort:
    Abort:
      m_state.store(ExecState::Aborted, std::memory_order_release);
      return true;
    case RequestType::Pause:
      m_state.store(ExecState::Paused, std::memory_order_release);

      // TODO(bastian): Might be worth experimenting with different pausing mechanisms. Basically
      // the longer we pause the slower we are at reacting to a 'resume' but the shorter we pause
      // the more cpu cycles we waste.
      //
      // Current strategy is we do a single longer pause (thread yield) and after returning from
      // that we do short cpu pauses until we are resumed. This works well if the pause request is
      // very short, but if its longer it starts to be wastefull.
      std::this_thread::yield();
      while (req = m_request.load(std::memory_order_acquire), req == RequestType::Pause) {
        _mm_pause();
      }
      if (unlikely(req == RequestType::Abort)) {
        goto Abort;
      }

      // Store running with sequential-consistency order and restart the trap check. This is
      // important because we could be re-paused in between us checking.
      m_state.store(ExecState::Running, std::memory_order_seq_cst);
      goto TrapBegin;
    case RequestType::None:
      return false;
    }

    // Unreachable as long as valid request types are used.
    assert(false);
    return false;
  }

  // Request the executor to abort.
  // NOTE: After requesting an abort it is unsafe to access the executor_handle anymore, as it can
  // destroy itself at any point after that.
  // NOTE: An aborted executor will NOT unregister itself anymore from the registry upon shutdown,
  // it is up to the caller to unregister it.
  inline auto requestAbort() noexcept {
    m_request.store(RequestType::Abort, std::memory_order_release);
  }

  // Request the executor to pause. Returns immediately with a boolean indicating if the executor
  // has paused yet. Common pattern is to keep calling this function until true is returned.
  inline auto requestPause() noexcept -> bool {
    // Set request to 'Pause' in case its currently 'None', reason is we want to leave it alone when
    // its currently set to 'Abort' to avoid resurrecting aborted executors.
    auto expectedReq = RequestType::None;
    m_request.compare_exchange_strong(
        expectedReq, RequestType::Pause, std::memory_order_acq_rel, std::memory_order_relaxed);
    return m_state.load(std::memory_order_acquire) != ExecState::Running;
  }

  inline auto resume() noexcept -> void {
    // Set request to 'None' in case its currently 'Pause', reason is we want to leave it alone when
    // its currently set to 'Abort' to avoid resurrecting aborted executors.
    auto expectedReq = RequestType::Pause;
    m_request.compare_exchange_strong(
        expectedReq, RequestType::None, std::memory_order_acq_rel, std::memory_order_relaxed);

    // Double check that we did not try to resume a non-paused executor.
    assert(expectedReq != RequestType::None);
  }

private:
  enum class RequestType : int {
    None  = 0,
    Abort = 1,
    Pause = 2,
  };

  BasicStack* m_stack;
  std::atomic<ExecState> m_state;
  std::atomic<RequestType> m_request;

  ExecutorHandle* m_prev;
  ExecutorHandle* m_next;
};

} // namespace vm::internal
