#include "vm/vm.hpp"
#include "internal/executor.hpp"
#include "internal/executor_registry.hpp"
#include "internal/interupt.hpp"
#include "internal/os_include.hpp"
#include "internal/platform_utilities.hpp"
#include "internal/ref_allocator.hpp"
#include "vm/platform_interface.hpp"
#include <csignal>

namespace vm {

#if defined(_WIN32)

static auto setupWinsock(internal::Settings* settings) noexcept {
  if (settings->socketsEnabled) {
    const auto reqWsaVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (::WSAStartup(reqWsaVersion, &wsaData) != 0) {
      settings->socketsEnabled = false;
    }
    // Verify that WSA 2.2 is supported.
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
      ::WSACleanup();
      settings->socketsEnabled = false;
    }
  }
}

static auto setupInputConsole(internal::Settings* settings, PlatformInterface* iface) noexcept {
  if (!::GetConsoleMode(iface->getStdIn(), &settings->win32OriginalInputConsoleMode)) {
    return false;
  }
  DWORD newInputConsoleMode = settings->win32OriginalInputConsoleMode;
  newInputConsoleMode |= 0x0001; // ENABLE_PROCESSED_INPUT 0x0001
  newInputConsoleMode |= 0x0200; // ENABLE_VIRTUAL_TERMINAL_INPUT 0x0200
  if (!::SetConsoleMode(iface->getStdIn(), newInputConsoleMode)) {
    return false;
  }
  if (!::SetConsoleCP(CP_UTF8)) {
    return false;
  }
  return true;
}

static auto setupOutputConsole(internal::Settings* settings, PlatformInterface* iface) noexcept {
  // Save the original console mode.
  if (!::GetConsoleMode(iface->getStdOut(), &settings->win32OriginalOutputConsoleMode)) {
    return false;
  }
  DWORD newOutputConsoleMode = settings->win32OriginalOutputConsoleMode;
  newOutputConsoleMode |= 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
  if (!::SetConsoleMode(iface->getStdOut(), newOutputConsoleMode)) {
    return false;
  }
  if (!::SetConsoleOutputCP(CP_UTF8)) {
    return false;
  }
  return true;
}

static auto
restoreInputConsole(const internal::Settings* settings, PlatformInterface* iface) noexcept {
  return ::SetConsoleMode(iface->getStdIn(), settings->win32OriginalInputConsoleMode) == 0;
}

static auto
restoreOutputConsole(const internal::Settings* settings, PlatformInterface* iface) noexcept {
  return ::SetConsoleMode(iface->getStdOut(), settings->win32OriginalOutputConsoleMode) == 0;
}

static auto setup(internal::Settings* settings, PlatformInterface* iface) noexcept {
  setupWinsock(settings);
  setupInputConsole(settings, iface);
  setupOutputConsole(settings, iface);
  internal::setupPlatformUtilities();

  if (settings->interceptInterupt) {
    settings->interceptInterupt = internal::interruptSetupHandler();
  }
}

static auto teardown(const internal::Settings* settings, PlatformInterface* iface) noexcept {
  if (settings->socketsEnabled) {
    ::WSACleanup();
  }
  restoreInputConsole(settings, iface);
  restoreOutputConsole(settings, iface);
  internal::teardownPlatformUtilities();
}

#else // !_WIN32

static auto setup(internal::Settings* settings, PlatformInterface* /*unused*/) noexcept {

  // Ignore sig-pipe (we want to handle it on a per call basis instead of globally).
  signal(SIGPIPE, SIG_IGN);

  internal::setupPlatformUtilities();

  if (settings->interceptInterupt) {
    settings->interceptInterupt = internal::interruptSetupHandler();
  }
}

static auto teardown(const internal::Settings* /*unused*/, PlatformInterface* /*unused*/) noexcept {
}

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

  setup(&settings, iface);

  auto resultState = execute(
      &settings,
      executable,
      iface,
      &execRegistry,
      &refAlloc,
      &gc,
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

  teardown(&settings, iface);

  return resultState;
}

} // namespace vm
