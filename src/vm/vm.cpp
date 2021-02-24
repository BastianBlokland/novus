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
    settings->interceptInterupt = internal::interruptSetupHandler();
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
    settings->interceptInterupt = internal::interruptSetupHandler();
  }
}

static auto teardown(const internal::Settings* /*unused*/) noexcept {}

#endif // !_WIN32

auto run(const novasm::Executable* executable, PlatformInterface* iface) noexcept -> ExecState {

  auto execRegistry = internal::ExecutorRegistry{};
  auto memAlloc     = internal::MemoryAllocator{};
  auto refAlloc     = internal::RefAllocator{&memAlloc};
  auto gc           = internal::GarbageCollector{&refAlloc, &execRegistry};

  if (unlikely(gc.startCollector() == internal::GarbageCollector::CollectorStartResult::Failure)) {
    return ExecState::VmInitFailed;
  }

  auto settings              = internal::Settings{};
  settings.socketsEnabled    = true; // TODO: Make configurable.
  settings.interceptInterupt = true; // TODO: Make configurable.

  setup(&settings);

  auto resultState = execute(
      &settings,
      executable,
      iface,
      &execRegistry,
      &refAlloc,
      executable->getEntrypoint(),
      0,
      nullptr,
      nullptr);

  // Terminate the garbage-collector (finishes any ongoing collections).
  gc.terminateCollector();

  // Abort all executors that are still running.
  // NOTE: First terminate the garbage collector as that might attempt to pause / resume executors
  // while we are trying to abort them.
  execRegistry.abortExecutors();

  assert(execRegistry.isAborted());

  teardown(&settings);

  return resultState;
}

} // namespace vm
