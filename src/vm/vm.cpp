#include "vm/vm.hpp"
#include "internal/executor.hpp"
#include "internal/executor_registry.hpp"
#include "internal/interupt.hpp"
#include "internal/os_include.hpp"
#include "internal/ref_allocator.hpp"
#include "vm/platform_interface.hpp"
#include <csignal>

namespace vm {

#if defined(_WIN32)

static auto setupWinsock(internal::Settings* settings) noexcept {
  if (settings->socketsEnabled) {
    const auto reqWsaVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(reqWsaVersion, &wsaData) != 0) {
      settings->socketsEnabled = false;
    }
    // Verify that WSA 2.2 is supported.
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
      WSACleanup();
      settings->socketsEnabled = false;
    }
  }
}

static auto enableVTConsoleMode() noexcept {
  const auto hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE) {
    return false;
  }
  DWORD dwMode = 0;
  if (!GetConsoleMode(hOut, &dwMode)) {
    return false;
  }
  dwMode |= 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
  return SetConsoleMode(hOut, dwMode) != 0;
}

static auto setup(internal::Settings* settings) noexcept {
  setupWinsock(settings);
  enableVTConsoleMode();

  if (settings->interceptInterupt) {
    settings->interceptInterupt = internal::setupInterruptHandler();
  }
}

static auto teardown(const internal::Settings* settings) noexcept {

  // Cleanup the winsock library.
  if (settings->socketsEnabled) {
    WSACleanup();
  }
}

#else // !_WIN32

static auto setup(internal::Settings* settings) noexcept {

  // Ignore sig-pipe (we want to handle it on a per call basis instead of globally).
  signal(SIGPIPE, SIG_IGN);

  if (settings->interceptInterupt) {
    settings->interceptInterupt = internal::setupInterruptHandler();
  }
}

static auto teardown(const internal::Settings* /*unused*/) noexcept {}

#endif // !_WIN32

auto run(const novasm::Assembly* assembly, PlatformInterface* iface) noexcept -> ExecState {

  auto settings              = internal::Settings{};
  settings.socketsEnabled    = true; // TODO: Make configurable.
  settings.interceptInterupt = true; // TODO: Make configurable.

  setup(&settings);

  auto execRegistry = internal::ExecutorRegistry{};
  auto memAlloc     = internal::MemoryAllocator{};
  auto refAlloc     = internal::RefAllocator{&memAlloc};
  auto gc           = internal::GarbageCollector{&refAlloc, &execRegistry};

  auto resultState = execute(
      settings,
      assembly,
      iface,
      &execRegistry,
      &refAlloc,
      assembly->getEntrypoint(),
      0,
      nullptr,
      nullptr);

  // Abort all executors that are still running.
  execRegistry.abortExecutors();

  // Terminate the garbage-collector (finishes any ongoing collections).
  gc.terminateCollector();

  teardown(&settings);

  return resultState;
}

} // namespace vm
