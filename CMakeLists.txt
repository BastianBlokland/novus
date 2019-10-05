cmake_minimum_required(VERSION 3.15)

project(
  Novus
  VERSION 0.1
  LANGUAGES CXX)

# Use the clang-tidy linter.
set(CMAKE_CXX_CLANG_TIDY clang-tidy)

# Output executables to bin dir.
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# Enable 'CTest' testing.
include(CTest)
enable_testing()

add_subdirectory(src)
add_subdirectory(apps)
add_subdirectory(tests)
