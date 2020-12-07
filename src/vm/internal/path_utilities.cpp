#include "internal/path_utilities.hpp"
#include "internal/os_include.hpp"

#if defined(_MSC_VER)

#define PATH_MAX MAX_PATH
#define getcwd _getcwd

#endif // _MSC_VER

namespace vm::internal {

auto getWorkingDirPath(RefAllocator* refAlloc) noexcept -> StringRef* {
  auto* str = refAlloc->allocStr(PATH_MAX);
  if (unlikely(str == nullptr)) {
    return nullptr;
  }

  if (getcwd(str->getCharDataPtr(), str->getSize()) == nullptr) {
    // Failed to get the current working directory, return an empty string.
    str->updateSize(0);
    return str;
  }

  str->updateSize(std::strlen(str->getCharDataPtr()));
  return str;
}

} // namespace vm::internal
