#if defined(linux) || defined(__linux__)
#include "internal/iowatcher_linux.cpp" // NOLINT
#elif defined(_WIN32)
#include "internal/iowatcher_win32.cpp" // NOLINT
#elif defined(__APPLE__)
#include "internal/iowatcher_macos.cpp" // NOLINT
#else
#include "internal/iowatcher_fallback.cpp" // NOLINT
#endif
