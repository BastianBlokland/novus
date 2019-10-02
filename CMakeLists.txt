cmake_minimum_required(VERSION 3.15)

project(
  Novus
  VERSION 0.1
  LANGUAGES CXX)

# Use the clang-tidy linter.
# Note: Pointer decay rules disabled because the 'assert' macro violates this.
set(CMAKE_CXX_CLANG_TIDY
  "clang-tidy;-checks=*,\
  -clang-analyzer-alpha.*,\
  -fuchsia-*,\
  -hicpp-no-array-decay,\
  -cppcoreguidelines-pro-bounds-array-to-pointer-decay,\
  -readability-implicit-bool-conversion")

# Output executables to bin dir.
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(src)
add_subdirectory(apps)
