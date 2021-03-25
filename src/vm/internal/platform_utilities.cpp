#include "internal/platform_utilities.hpp"
#include "internal/intrinsics.hpp"
#include "internal/os_include.hpp"

namespace vm::internal {

namespace {

Endianness g_endianness;

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

} // namespace

auto setupPlatformUtilities() noexcept -> void {
  initEndianness();
#if defined(_WIN32)
  initQueryPerfCounterFrequency();
#endif
}

auto getEndianness() noexcept -> Endianness { return g_endianness; }

auto clockMicroSinceEpoch() noexcept -> int64_t {

#if defined(_WIN32)

  // Windows FILETIME is in 100 ns ticks since January 1 1601.

  constexpr int64_t winEpochToUnixEpoch = 116'444'736'000'000'000LL;
  constexpr int64_t winTickToMicro      = 10LL;

  FILETIME fileTime;
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
  ::GetSystemTimePreciseAsFileTime(&fileTime);
#else
  ::GetSystemTimeAsFileTime(&fileTime);
#endif
  LARGE_INTEGER winTicks;
  winTicks.LowPart  = fileTime.dwLowDateTime;
  winTicks.HighPart = fileTime.dwHighDateTime;

  return (winTicks.QuadPart - winEpochToUnixEpoch) / winTickToMicro;

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
  if (unlikely(::GetTimeZoneInformation(&timeZoneInfo) == TIME_ZONE_ID_INVALID)) {
    // TODO: Consider if we want to swallow this or if we add error reporting to this call.
    return 0;
  }
  return -static_cast<int32_t>(timeZoneInfo.Bias);

#else // !_WIN32

  const time_t utcSeconds            = ::time(nullptr);
  const time_t localSeconds          = ::timegm(::localtime(&utcSeconds));
  const time_t timezoneOffsetSeconds = localSeconds - utcSeconds;
  const time_t timezoneOffsetMinutes = timezoneOffsetSeconds / 60;
  return static_cast<int32_t>(timezoneOffsetMinutes);

#endif
}

} // namespace vm::internal
