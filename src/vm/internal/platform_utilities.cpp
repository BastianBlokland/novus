#include "internal/platform_utilities.hpp"
#include "internal/intrinsics.hpp"
#include "internal/os_include.hpp"
#include "internal/string_utilities.hpp"

#if defined(_MSC_VER)

#define PATH_MAX MAX_PATH
#define getcwd _getcwd

#endif // _MSC_VER

namespace vm::internal {

namespace {

Endianness g_endianness;
char* g_workingDir; // Cache of the working directory, to avoid making sys calls every time.
size_t g_workingDirSize;

#if defined(_WIN32)
// Frequency of the performance counter, value is consistent after boot so can be cached.
int64_t g_winPerfCounterFrequency; // Ticks per second.

auto initQueryPerfCounterFrequency() noexcept -> void {
  LARGE_INTEGER freq;
  if (likely(::QueryPerformanceFrequency(&freq))) {
    g_winPerfCounterFrequency = freq.QuadPart;
  } else {
    g_winPerfCounterFrequency = 1;
  }
}
#endif

auto initEndianness() noexcept -> void {
  union Mix {
    uint32_t i;
    uint8_t c[4]; // NOLINT(modernize-avoid-c-arrays)
  };

  auto e       = Mix{0x01'00'00'00};
  g_endianness = e.c[0] ? Endianness::Big : Endianness::Little;
}

auto initWorkingDirectory() noexcept -> void {
  if (g_workingDir) {
    ::free(const_cast<char*>(g_workingDir));
  }
  g_workingDir     = ::getcwd(nullptr, 0);
  g_workingDirSize = g_workingDir ? ::strlen(g_workingDir) : 0;
}

auto teardownWorkingDirectory() noexcept -> void {
  ::free(const_cast<char*>(g_workingDir));
  g_workingDir     = nullptr;
  g_workingDirSize = 0;
}

} // namespace

auto setupPlatformUtilities() noexcept -> void {
  initEndianness();
  initWorkingDirectory();

#if defined(_WIN32)
  initQueryPerfCounterFrequency();
#endif
}

auto teardownPlatformUtilities() noexcept -> void { teardownWorkingDirectory(); }

auto getEndianness() noexcept -> Endianness { return g_endianness; }

auto clockMicroSinceEpoch() noexcept -> int64_t {

#if defined(_WIN32)

  FILETIME fileTime;
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
  ::GetSystemTimePreciseAsFileTime(&fileTime);
#else
  ::GetSystemTimeAsFileTime(&fileTime);
#endif
  return winFileTimeToMicroSinceEpoch(fileTime);

#else // !_WIN32

  timespec ts;
  if (unlikely(::clock_gettime(CLOCK_REALTIME, &ts))) {
    return -1;
  }
  return ts.tv_sec * 1'000'000L + ts.tv_nsec / 1'000;

#endif
}

auto clockNanoSteady() noexcept -> int64_t {

#if defined(_WIN32)

  LARGE_INTEGER prefTicks;
  if (unlikely(!::QueryPerformanceCounter(&prefTicks))) {
    return -1;
  }
  return prefTicks.QuadPart * 1'000'000LL / g_winPerfCounterFrequency * 1'000;

#else // !_WIN32

  timespec ts;
  if (unlikely(::clock_gettime(CLOCK_MONOTONIC, &ts))) {
    return -1;
  }
  return ts.tv_sec * 1'000'000'000L + ts.tv_nsec;

#endif
}

auto clockTimezoneOffset() noexcept -> int32_t {

#if defined(_WIN32)

  TIME_ZONE_INFORMATION timeZoneInfo;
  switch (::GetTimeZoneInformation(&timeZoneInfo)) {
  case TIME_ZONE_ID_STANDARD:
    return -static_cast<int32_t>(timeZoneInfo.Bias + timeZoneInfo.StandardBias);
  case TIME_ZONE_ID_DAYLIGHT:
    return -static_cast<int32_t>(timeZoneInfo.Bias + timeZoneInfo.DaylightBias);
  case TIME_ZONE_ID_INVALID:
  default:
    // TODO: Consider if we want to swallow this or if we add error reporting to this call.
    return 0;
  }

#else // !_WIN32

  const time_t utcSeconds            = ::time(nullptr);
  const time_t localSeconds          = ::timegm(::localtime(&utcSeconds));
  const time_t timezoneOffsetSeconds = localSeconds - utcSeconds;
  const time_t timezoneOffsetMinutes = timezoneOffsetSeconds / 60;
  return static_cast<int32_t>(timezoneOffsetMinutes);

#endif
}

auto platformHasEnv(const StringRef* name) noexcept -> bool {
  return ::getenv(name->getCharDataPtr());
}

auto platformGetEnv(const StringRef* name, RefAllocator* refAlloc) noexcept -> StringRef* {
  const char* envVar = ::getenv(name->getCharDataPtr());
  return envVar ? toStringRef(refAlloc, envVar) : refAlloc->allocStr(0);
}

auto platformWorkingDirPath(RefAllocator* refAlloc) noexcept -> StringRef* {
  // NOTE: This is only safe as long as we don't expose any way to update the working directory at
  // runtime.
  return refAlloc->allocStrLit(g_workingDir, g_workingDirSize);
}

auto platformExecPath(RefAllocator* refAlloc) noexcept -> StringRef* {
  auto* str = refAlloc->allocStr(PATH_MAX);
  if (unlikely(str == nullptr)) {
    return nullptr;
  }

#if defined(linux)

  constexpr auto selfLink = "/proc/self/exe";
  if (realpath(selfLink, str->getCharDataPtr()) == nullptr) {
    // Failed to resolve the self symlink, return an empty string.
    str->updateSize(0);
    return str;
  }
  const auto size = std::strlen(str->getCharDataPtr());

#elif defined(__APPLE__) // !linux

  uint32_t tempBufferSize = PATH_MAX;
  auto* tempBuffer        = static_cast<char*>(std::malloc(tempBufferSize));
  if (unlikely(tempBuffer == nullptr)) {
    return nullptr;
  }
  if (_NSGetExecutablePath(tempBuffer, &tempBufferSize)) {
    // Failed to get the executable path, return an empty string.
    std::free(tempBuffer);
    str->updateSize(0);
    return str;
  }
  if (realpath(tempBuffer, str->getCharDataPtr()) == nullptr) {
    // Failed to resolve the resulting path, return an empty string.
    std::free(tempBuffer);
    str->updateSize(0);
    return str;
  }
  std::free(tempBuffer);
  const auto size = std::strlen(str->getCharDataPtr());

#elif defined(_WIN32) // !linux && !__APPLE__

  const auto size = GetModuleFileName(nullptr, str->getCharDataPtr(), str->getSize());
  if (size == 0 || size == str->getSize()) {
    // Failed to get the module filename, return an empty string.
    str->updateSize(0);
    return str;
  }

#endif // _WIN32

  str->updateSize(size);
  return str;
}

} // namespace vm::internal
