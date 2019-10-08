cmake_minimum_required(VERSION 3.15)

project(
  Novus
  VERSION 0.1
  LANGUAGES CXX)

  # Generate a 'compile_commands.json' for use by clangd.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use the clang-tidy linter if installed.
find_program(CLANG_TIDY_BIN NAMES "clang-tidy")
if(CLANG_TIDY_BIN)
  set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_BIN})
  message("Enabled clang-tidy.")
endif()

# Output executables to bin dir.
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# Enable 'CTest' testing.
include(CTest)
enable_testing()

add_subdirectory(src)
add_subdirectory(apps)
add_subdirectory(tests)
