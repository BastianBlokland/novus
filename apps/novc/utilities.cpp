#include "utilities.hpp"

#if !defined(_WIN32)
#include <sys/stat.h>
#endif

namespace novc {

#if defined(_WIN32)

auto setOutputFilePermissions(filesystem::path /*unused*/) noexcept -> bool {
  // No-op on Windows atm.
  return true;
}

#else // !_WIN32

auto setOutputFilePermissions(filesystem::path filePath) noexcept -> bool {
  mode_t permMode = 0;
  permMode |= S_IRUSR | S_IWUSR | S_IXUSR; // Read, Write, Execute by owner.
  permMode |= S_IRGRP | S_IWGRP;           // Read, Write by group.
  permMode |= S_IROTH;                     // Read by others.
  return ::chmod(filePath.c_str(), permMode) == 0;
}

#endif // !_WIN32

} // namespace novc
