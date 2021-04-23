#include "input/search_paths.hpp"
#include <array>
#include <optional>
#include <string>

#if defined(linux)
#include <climits>
#include <unistd.h>
#endif

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#include <unistd.h>
#endif

#if defined(_WIN32)
#include <direct.h>
#include <windows.h>
#endif

#if defined(_MSC_VER)
#define PATH_MAX MAX_PATH
#define getcwd _getcwd
#endif

namespace {

auto errorExit(const char* msg) {
  std::fprintf(stderr, "%s\n", msg);
  std::fflush(stderr);
  std::exit(EXIT_FAILURE);
}

auto workingDirectory() -> std::optional<filesystem::path> {
  auto pathBuffer = std::array<char, PATH_MAX>{};
  if (::getcwd(pathBuffer.data(), PATH_MAX)) {
    return filesystem::path{pathBuffer.data()};
  }
  return std::nullopt;
}

} // namespace

namespace input {

auto getSearchPaths(const char** argv) noexcept -> std::vector<filesystem::path> {
  auto result = std::vector<filesystem::path>{};

  if (auto wd = workingDirectory()) {
    result.push_back(std::move(*wd));
  }

  // If the executable was started with a path add it as a search-path.
  // Note: if the executable was found from PATH this will only contain the executable name so we
  // cannot find a directory in that case.
  if (filesystem::exists(argv[0])) {
    auto parent = filesystem::absolute(argv[0]).parent_path();
    if (filesystem::is_directory(parent)) {
      parent = filesystem::canonical(parent);
      result.push_back(std::move(parent));
    }
  }

  // Add the parent path of the executable.
  auto executablePath = getExecutablePath();
  result.push_back(filesystem::canonical(executablePath.parent_path()));

  return result;
}

auto getExecutablePath() noexcept -> filesystem::path {
#if defined(linux)

  constexpr auto selfLink = "/proc/self/exe";

  std::error_code err;
  auto path = filesystem::read_symlink(selfLink, err);
  if (err) {
    errorExit("Failed to read '/proc/self/exe'");
  }
  return path;

#elif defined(__APPLE__) // !linux

  unsigned int pathBufferSize = PATH_MAX;
  auto pathBuffer             = std::array<char, PATH_MAX>{};
  if (_NSGetExecutablePath(pathBuffer.data(), &pathBufferSize)) {
    errorExit("Failed to retrieve executable path");
  }
  return filesystem::path{pathBuffer.data()};

#elif defined(_WIN32) // !linux && !__APPLE__

  auto pathBuffer = std::array<char, PATH_MAX>{};
  auto size       = GetModuleFileName(nullptr, pathBuffer.data(), PATH_MAX);
  if (!size) {
    errorExit("Failed to retrieve executable path");
  }
  return filesystem::path{pathBuffer.data()};

#endif
}

} // namespace input
