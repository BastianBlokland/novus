#include "input/search_paths.hpp"
#include <string>

namespace input {

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
  if (filesystem::exists(selfLink)) {
    auto parentPath = filesystem::read_symlink(selfLink).parent_path();
    if (filesystem::is_directory(parentPath)) {
      result.push_back(std::move(parentPath));
    }
  }
#endif

  return result;
}

} // namespace input
