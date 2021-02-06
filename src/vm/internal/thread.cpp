#include "internal/thread.hpp"
#include "internal/os_include.hpp"
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <thread>

namespace vm::internal {

auto threadStart(ThreadRoutineRaw routine, void* arg) noexcept -> ThreadStartResult {
  auto res = ThreadStartResult::Success;

#if defined(_WIN32)

  const auto threadHandle = CreateThread(nullptr, 0, routine, arg, 0, nullptr);
  if (!threadHandle) {
    return ThreadStartResult::Failure;
  }
  // Close the handle immediately as we want to create a detacted thread.
  if (!CloseHandle(threadHandle)) {
    return ThreadStartResult::Failure;
  }

#else // !_WIN32

  pthread_attr_t attr;
  pthread_t threadId;

  if (pthread_attr_init(&attr) != 0) {
    return ThreadStartResult::Failure;
  }
  if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
    return ThreadStartResult::Failure;
  }
  if (pthread_create(&threadId, &attr, routine, arg) != 0) {
    res = ThreadStartResult::Failure;
  }
  if (pthread_attr_destroy(&attr) != 0) {
    return ThreadStartResult::Failure;
  }

#endif
  return res;
}

auto threadYield() noexcept -> void {
#if defined(_WIN32)
  SwitchToThread();
#else // !_WIN32
  sched_yield();
#endif
}

auto threadSleepNano(int64_t time) noexcept -> void {
#if defined(_WIN32)

  // TODO: This only has milliseconds resolution, investigate win32 alternatives with better
  // resolution.
  unsigned long timeMilli = static_cast<unsigned long>(time) / 1'000'000;
  Sleep(timeMilli > 0 ? timeMilli : 1);

#else // !_WIN32

  timespec ts = {time / 1'000'000'000, time % 1'000'000'000};
  while (nanosleep(&ts, &ts) == -1 && errno == EINTR) // Resume waiting after interupt.
    ;

#endif
}

} // namespace vm::internal
