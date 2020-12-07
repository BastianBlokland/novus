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

auto getExecPath(RefAllocator* refAlloc) noexcept -> StringRef* {
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
  auto* tempBuffer    = static_cast<char*>(std::malloc(tempBufferSize));
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
