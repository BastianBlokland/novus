#include "input/search_paths.hpp"
#include <string>

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

namespace input {

static auto addExecutableParent(filesystem::path p, std::vector<filesystem::path>* paths) noexcept {
  if (!filesystem::exists(p)) {
    return;
  }

  // If the path is a symlink then resolve it.
  if (filesystem::is_symlink(p)) {
    p = filesystem::read_symlink(p);
  }

  // Add the parent of the path.
  paths->push_back(p.parent_path());
}

auto getSearchPaths(char** argv) noexcept -> std::vector<filesystem::path> {
  auto result = std::vector<filesystem::path>{};

  // If the executable was started with a path add it as a search-path.
  // Note: if the executable was found from PATH this will only contain the executable name so we
  // cannot find a directory in that case.
  if (filesystem::exists(argv[0])) {
    auto parent = filesystem::absolute(argv[0]).parent_path();
    if (filesystem::is_directory(parent)) {
      result.push_back(std::move(parent));
    }
  }

#if defined(linux)
  // On linux we can use the '/proc/self/exe' symlink to find the path to our executable.
  const static auto selfLink = "/proc/self/exe";
  addExecutableParent(selfLink, &result);

#elif defined(__APPLE__)
  // On osx we call a os specific api to get the executable path.
  unsigned int pathBufferSize = PATH_MAX;
  auto pathBuffer             = std::array<char, PATH_MAX>{};
  if (_NSGetExecutablePath(pathBuffer.data(), &pathBufferSize) == 0) {
    addExecutableParent(filesystem::path{pathBuffer.data()}, &result);
  }
#endif

  return result;
}

} // namespace input
