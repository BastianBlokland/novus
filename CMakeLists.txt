cmake_minimum_required(VERSION 3.15)

project(
  Novus
  VERSION 0.1
  LANGUAGES CXX)

message("Compiler: ${CMAKE_CXX_COMPILER_ID}")

# Output executables to bin dir.
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# Enable 'CTest' testing.
include(CTest)
enable_testing()

# Global compiler flags.
if(MSVC)
  message("Setting msvc compiler flags")
  set(CMAKE_CXX_FLAGS "/std:c++17")
else()
  set(CXX_FLAGS_SHARED "-std=c++17 -Werror -Wall -Wextra -fno-strict-aliasing")
  set(CMAKE_CXX_FLAGS_DEBUG "-O1 -g -fno-omit-frame-pointer ${CXX_FLAGS_SHARED}")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG ${CXX_FLAGS_SHARED}")
endif()

# Older version of libstdc++ require linking against the <experimental/filesystem> library directly.
# TODO(bastian): Investigate if there is a better way to handle this.
if(UNIX AND NOT APPLE)
  message("Add linking to stdc++fs")
  link_libraries(stdc++fs)
endif()

# Find the threads library.
set(CMAKE_THREAD_PREFER_PTHREAD On)
find_package(Threads REQUIRED)

# Generate a 'compile_commands.json' for use by clangd.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Instead of a 'includes_CXX.rsp' file just pass the includes,
# reason is that 'mingw64-w64' does not respect that file.
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES OFF)

# Use the clang-tidy linter if installed.
find_program(CLANG_TIDY_BIN NAMES "clang-tidy" "clang-tidy-9")
if(CLANG_TIDY_BIN)
  set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_BIN})
  message("Enabled clang-tidy")
endif()

# Add child cmake files.
add_subdirectory(src)
add_subdirectory(apps)
add_subdirectory(novstd)

if(BUILD_TESTING)
  message("Including compiler and vm tests")
  add_subdirectory(tests)
endif()
