#if defined(linux)
#include "internal/iowatcher_linux.cpp" // NOLINT
#endif

#if defined(_WIN32)
#include "internal/iowatcher_win32.cpp" // NOLINT
#endif

#if defined(__APPLE__)
#include "internal/iowatcher_macos.cpp" // NOLINT
#endif
