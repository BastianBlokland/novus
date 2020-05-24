cmake_minimum_required(VERSION 3.15)

project(
  Novus
  VERSION 0.3.0
  LANGUAGES CXX)

# Custom options options
set(LINTING "Off" CACHE BOOL "Should source linting be enabled")

# Print some diagnostic information.
message(STATUS "Configuring Novus")
message(STATUS "* Host system: ${CMAKE_HOST_SYSTEM}")
message(STATUS "* Host processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "* CMake version: ${CMAKE_VERSION}")
message(STATUS "* Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "* Linting: ${LINTING}")
message(STATUS "* Source path: ${PROJECT_SOURCE_DIR}")
message(STATUS "* Build path: ${PROJECT_BINARY_DIR}")
message(STATUS "* Ouput path: ${PROJECT_SOURCE_DIR}/bin")
message(STATUS "* Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "* Generator: ${CMAKE_GENERATOR}")

# Output executables to bin dir.
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${PROJECT_SOURCE_DIR}/bin>)

# Enable 'CTest' testing.
message(STATUS "Enable CTest dependency")
include(CTest)
enable_testing()

# Global compiler flags.
if(MSVC)
  message(STATUS "Setting msvc compiler flags")
  set(CMAKE_CXX_FLAGS "/std:c++17 /WX /wd4530 /wd26451")
else()
  message(STATUS "Setting unix compiler flags")
  set(CXX_FLAGS_SHARED "-std=c++17 -Werror -Wall -Wextra -fno-strict-aliasing")
  set(CMAKE_CXX_FLAGS_DEBUG "-O1 -g -fno-omit-frame-pointer ${CXX_FLAGS_SHARED}")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG ${CXX_FLAGS_SHARED}")
endif()

# Older version of libstdc++ require linking against the <experimental/filesystem> library directly.
# TODO(bastian): Investigate if there is a better way to handle this.
if((UNIX AND NOT APPLE) OR MINGW)
  message(STATUS "Add linking to stdc++fs (filesystem library)")
  link_libraries(stdc++fs)
endif()

# Find the threads library.
message(STATUS "Finding threads package")
set(CMAKE_THREAD_PREFER_PTHREAD On)
find_package(Threads REQUIRED)

# Windows speficic settings.
if(WIN32)

  # On windows link against the 'winsock' library.
  link_libraries(ws2_32)

  # Target windows 'vista', reason is some additional winsock api's became available in vista.
  add_definitions(-DWINVER=0x0600 -D_WIN32_WINNT=0x0600)
endif()

# Generate a 'compile_commands.json' for use by clangd.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Instead of a 'includes_CXX.rsp' file just pass the includes,
# reason is that MinGW does not respect that file.
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES OFF)

if(LINTING)
  # Use the clang-tidy linter if installed.
  find_program(CLANG_TIDY_BIN NAMES "clang-tidy" "clang-tidy-9")
  if(CLANG_TIDY_BIN)
    message(STATUS "Enabling clang-tidy linter")
    set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_BIN})
  endif()
endif()

# Write a 'VERSION' file to the bin dir with the cmake project version.
file(WRITE ${EXECUTABLE_OUTPUT_PATH}/VERSION ${CMAKE_PROJECT_VERSION})

# Replace config variables in the config header.
configure_file("include/config.hpp.in" "${PROJECT_SOURCE_DIR}/include/config.hpp")

# Add child cmake files.
add_subdirectory(src)
add_subdirectory(apps)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

add_subdirectory(novstd)
