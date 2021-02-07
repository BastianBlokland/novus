#pragma once
#include <cstdint>
#include <immintrin.h>
#include <tuple>
#include <utility>

namespace vm::internal {

namespace details {

// At the moment these return values are not important as they are never retrievable.
#if defined(_WIN32)
using RawRoutineRetType = unsigned long; // On windows thread routines return a 'DWORD'.
#else // !_WIN32
using RawRoutineRetType = void*; // On unix thread routines return a opaque pointer.
#endif

template <typename RetT, typename... TArgs>
using Routine = RetT (*)(TArgs...) noexcept;

template <typename RetT, typename... TArgs>
using Invokable = std::tuple<Routine<RetT, TArgs...>, TArgs...>;

template <typename RetT, typename... TArgs, std::size_t... TupleIndices>
auto invoke(Invokable<RetT, TArgs...> invokable, std::index_sequence<TupleIndices...>) -> RetT {
  Routine<RetT, TArgs...> routine = std::get<0>(invokable);
  return routine(std::get<TupleIndices + 1>(invokable)...);
}

template <typename RetT, typename... TArgs>
auto invoke(Invokable<RetT, TArgs...> invokable) -> RetT {
  return invoke(invokable, std::index_sequence_for<TArgs...>{});
}

} // namespace details

template <typename RetT, typename... TArgs>
using ThreadRoutine = details::Routine<RetT, TArgs...>;

using ThreadRoutineRaw = details::Routine<details::RawRoutineRetType, void*>;

enum class ThreadStartResult {
  Success = 0,
  Failure = 1,
};

// Start a new thread that calls routine with the given args.
// NOTE: the return type of the routine is not used at this time.
template <typename RetT, typename... TArgs>
[[nodiscard]] auto threadStart(ThreadRoutine<RetT, TArgs...> routine, TArgs... args) noexcept
    -> ThreadStartResult {

  using Invokeable = details::Invokable<RetT, TArgs...>;

  // Create an invokeable on the heap (containing the routine and the args).
  auto* dataPtr = new Invokeable(routine, std::forward<TArgs>(args)...);

  // Define a type-erased function that takes an invokable, invokes it and then destroys it.
  constexpr ThreadRoutineRaw routineWrapper = [](void* rawArg) noexcept {
    auto* invokeablePtr = static_cast<Invokeable*>(rawArg);
    details::invoke(*invokeablePtr);
    delete invokeablePtr; // Cleanup the heap allocated invokable.
    return details::RawRoutineRetType{};
  };

  return threadStart(routineWrapper, dataPtr);
}

// Start a new thread that calls routine with the given arg.
// NOTE: the return type of the routine is not used at this time.
[[nodiscard]] auto threadStart(ThreadRoutineRaw routine, void* arg) noexcept -> ThreadStartResult;

// Place this thread at the bottom of the run queue.
auto threadYield() noexcept -> void;

// Sleep the current thread for the given amount of nanaseconds.
auto threadSleepNano(int64_t time) noexcept -> bool;

// Emit a cpu pause instruction.
inline auto threadPause() noexcept -> void { _mm_pause(); }

} // namespace vm::internal
