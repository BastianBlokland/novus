cmake_minimum_required(VERSION 3.15)

project(
  Novus
  VERSION 0.1
  LANGUAGES CXX)

# Custom options options
set(LINTING "Off" CACHE BOOL "Should source linting be enabled")

# Print some diagnostic information.
message(">> Configuring Novus")
message("-- Host system: ${CMAKE_HOST_SYSTEM}")
message("-- Host processor: ${CMAKE_SYSTEM_PROCESSOR}")
message("-- CMake version: ${CMAKE_VERSION}")
message("-- Build type: ${CMAKE_BUILD_TYPE}")
message("-- Linting: ${LINTING}")
message("-- Source path: ${PROJECT_SOURCE_DIR}")
message("-- Build path: ${PROJECT_BINARY_DIR}")
message("-- Ouput path: ${PROJECT_SOURCE_DIR}/bin")
message("-- Compiler: ${CMAKE_CXX_COMPILER_ID}")
message("-- Generator: ${CMAKE_GENERATOR}")

# Output executables to bin dir.
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# Enable 'CTest' testing.
message(">> Enable CTest dependency")
include(CTest)
enable_testing()

# Global compiler flags.
if(MSVC)
  message(">> Setting msvc compiler flags")
  set(CMAKE_CXX_FLAGS "/std:c++17")
else()
  message(">> Setting unix compiler flags")
  set(CXX_FLAGS_SHARED "-std=c++17 -Werror -Wall -Wextra -fno-strict-aliasing")
  set(CMAKE_CXX_FLAGS_DEBUG "-O1 -g -fno-omit-frame-pointer ${CXX_FLAGS_SHARED}")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG ${CXX_FLAGS_SHARED}")
endif()

# Older version of libstdc++ require linking against the <experimental/filesystem> library directly.
# TODO(bastian): Investigate if there is a better way to handle this.
if(UNIX AND NOT APPLE)
  message(">> Add linking to stdc++fs (filesystem library)")
  link_libraries(stdc++fs)
endif()

# Find the threads library.
message(">> Finding threads package")
set(CMAKE_THREAD_PREFER_PTHREAD On)
find_package(Threads REQUIRED)

# Generate a 'compile_commands.json' for use by clangd.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Instead of a 'includes_CXX.rsp' file just pass the includes,
# reason is that 'mingw64-w64' does not respect that file.
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES OFF)

if(LINTING)
  # Use the clang-tidy linter if installed.
  find_program(CLANG_TIDY_BIN NAMES "clang-tidy" "clang-tidy-9")
  if(CLANG_TIDY_BIN)
    message(">> Enabling clang-tidy linter")
    set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_BIN})
  endif()
endif()

# Add child cmake files.
add_subdirectory(src)
add_subdirectory(apps)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

add_subdirectory(novstd)
