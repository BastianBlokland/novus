#pragma once

#if __has_include(<filesystem>)

#include <filesystem>
namespace filesystem = std::filesystem;
#define HAS_FILESYSTEM

#elif __has_include(<experimental/filesystem>)

#include <experimental/filesystem>
namespace filesystem = std::experimental::filesystem;
#define HAS_FILESYSTEM

#endif
