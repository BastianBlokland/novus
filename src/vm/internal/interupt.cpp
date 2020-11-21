#include "internal/interupt.hpp"
#include "internal/os_include.hpp"
#include <atomic>
#include <csignal>

namespace vm::internal {

static std::atomic_bool g_interruptRequested = false;

auto interuptIsRequested() noexcept -> bool {
  return g_interruptRequested.load(std::memory_order_acquire);
}

auto interuptResetRequested() noexcept -> bool {
  g_interruptRequested.store(false, std::memory_order_release);
  return true; // Atm this cannot fail.
}

#if defined(_WIN32)

auto interruptHandler(DWORD dwCtrlType) noexcept -> int {
  switch (dwCtrlType) {
  case CTRL_C_EVENT:
  case CTRL_BREAK_EVENT:
  case CTRL_CLOSE_EVENT:
    g_interruptRequested.store(true, std::memory_order_release);
    return true; // Indicate that we have handled the event.
  default:
    return false; // Indicate that we have not handled the event.
  }
}

auto interruptSetupHandler() noexcept -> bool {
  return SetConsoleCtrlHandler(interruptHandler, true);
}

#else // !_WIN32

auto interruptHandler(int /*unused*/) noexcept -> void {
  g_interruptRequested.store(true, std::memory_order_release);
}

auto interruptSetupHandler() noexcept -> bool {

  struct sigaction actionInfo = {};
  actionInfo.sa_handler       = interruptHandler;
  sigemptyset(&actionInfo.sa_mask);

  return sigaction(SIGINT, &actionInfo, nullptr) == 0;
}

#endif // !_WIN32

} // namespace vm::internal
