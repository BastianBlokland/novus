cmake_minimum_required(VERSION 3.15)

cmake_policy(SET CMP0091 NEW)
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.19")
  # Note: Eventually we want to opt into the new behaviour for test names, but because thats only
  # introduced in 3.19 we stick to the old behaviour until that is commonly available.
  cmake_policy(SET CMP0110 OLD)
endif()

project(
  Novus
  VERSION 0.15.0
  LANGUAGES CXX)

# Custom options options.
set(BUILD_TESTING "Off" CACHE BOOL "Should test targets be build")
set(BUILD_FUZZING "Off" CACHE BOOL "Should fuzzing targets be build")
set(LINTING       "Off" CACHE BOOL "Should source linting be enabled")
set(SANITIZE      "Off" CACHE BOOL "Should santiser instrumentation be included in targets")
set(COVERAGE      "Off" CACHE BOOL "Should coverage instrumentation be included in targets")

# Print some diagnostic information.
message(STATUS "Configuring Novus")
message(STATUS "* Host system: ${CMAKE_HOST_SYSTEM}")
message(STATUS "* Host processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "* CMake version: ${CMAKE_VERSION}")
message(STATUS "* Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "* Build tests: ${BUILD_TESTING}")
message(STATUS "* Build fuzzing: ${BUILD_FUZZING}")
message(STATUS "* Linting: ${LINTING}")
message(STATUS "* Sanitize: ${SANITIZE}")
message(STATUS "* Coverage: ${COVERAGE}")
message(STATUS "* Source path: ${PROJECT_SOURCE_DIR}")
message(STATUS "* Build path: ${PROJECT_BINARY_DIR}")
message(STATUS "* Ouput path: ${PROJECT_SOURCE_DIR}/bin")
message(STATUS "* Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "* Generator: ${CMAKE_GENERATOR}")

# Output executables to bin dir.
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${PROJECT_SOURCE_DIR}/bin>)
file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# Enable 'CTest' testing.
message(STATUS "Enable CTest dependency")
include(CTest)
enable_testing()

if("${CMAKE_GENERATOR}" MATCHES "Visual Studio")
  message(STATUS "Configuring Visual Studio generator")
  SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
endif()

# Setup global compiler and linker flags.
set(CXX_FLAGS_SHARED "")
set(LINKER_FLAGS_SHARED "")
if(SANITIZE)
  if(NOT "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    message(FATAL_ERROR "Sanitizer instrumentation is only supported with the clang compiler")
  endif()
  if(WIN32)
    message(FATAL_ERROR "Sanitizer instrumentation is not supported on windows")
  endif()
  message(STATUS "Enabling sanitizer instrumentation")
  set(SANITIZERS "address,alignment,bool,builtin,bounds,enum,function,integer-divide-by-zero,object-size,return,unreachable")
  set(CXX_FLAGS_SHARED "${CXX_FLAGS_SHARED} -fsanitize=${SANITIZERS}")
  set(LINKER_FLAGS_SHARED "${LINKER_FLAGS_SHARED} -fsanitize=${SANITIZERS}")
endif()

if(COVERAGE)
  if(NOT "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    message(FATAL_ERROR "Coverage instrumentation is only supported with the clang compiler")
  endif()
  if(WIN32)
    message(FATAL_ERROR "Coverage instrumentation is not supported on windows")
  endif()
  message(STATUS "Enabling coverage instrumentation")
  set(CXX_FLAGS_SHARED "${CXX_FLAGS_SHARED} -fprofile-instr-generate -fcoverage-mapping")
  set(LINKER_FLAGS_SHARED "${LINKER_FLAGS_SHARED} -fprofile-instr-generate")
endif()

if(WIN32)
  message(STATUS "Enabling windows specific defines")
  set(CXX_FLAGS_SHARED "${CXX_FLAGS_SHARED} \
    -DNOMINMAX \
    -D_CRT_SECURE_NO_WARNINGS \
    -D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING")
endif()

if(MSVC)
  message(STATUS "Setting msvc compiler flags")
  set(CMAKE_MSVC_RUNTIME_LIBRARY "") # Manually configure compiler flags.
  set(CXX_FLAGS_SHARED "${CXX_FLAGS_SHARED} /std:c++17 /WX /wd4530 /wd26451")
  set(CMAKE_CXX_FLAGS_DEBUG "${CXX_FLAGS_SHARED} /O1 /MTd")
  set(CMAKE_CXX_FLAGS_RELEASE "${CXX_FLAGS_SHARED} /O2 /MT /DNDEBUG")
  set(CMAKE_EXE_LINKER_FLAGS "${LINKER_FLAGS_SHARED}")
else()
  message(STATUS "Setting unix compiler flags")
  set(CXX_FLAGS_SHARED "${CXX_FLAGS_SHARED} -std=c++17 -Werror -Wall -Wextra -fno-strict-aliasing")
  if(CMAKE_COMPILER_IS_GNUCXX OR MINGW)
    message(STATUS "Setting gcc specific compiler flags")
    # Disable 'maybe-uninitialized' warning on GCC, to many false positives at the time of writing.
    set(CXX_FLAGS_SHARED "${CXX_FLAGS_SHARED} -Wno-maybe-uninitialized")
  endif()
  set(CMAKE_CXX_FLAGS_DEBUG "-O1 -g -fno-omit-frame-pointer ${CXX_FLAGS_SHARED}")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG ${CXX_FLAGS_SHARED}")
  set(CMAKE_EXE_LINKER_FLAGS "${LINKER_FLAGS_SHARED}")
endif()

# (Older versions of?) libstdc++ require linking against the filesystem library explicitly.
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

  # Target windows '8', change to '0x0600' for supporting back to windows Vista but losing extra
  # precision in clock systemcalls. Further back then Vista we cannot easily support due to missing
  # winsock apis.
  add_definitions(-DWINVER=0x0602 -D_WIN32_WINNT=0x0602)
endif()

# Generate a 'compile_commands.json' for use by clangd.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Instead of a 'includes_CXX.rsp' file just pass the includes,
# reason is that MinGW does not respect that file.
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES OFF)

if(LINTING)
  # Use the clang-tidy linter if installed.
  find_program(CLANG_TIDY_BIN NAMES "clang-tidy")
  if(CLANG_TIDY_BIN)
    message(STATUS "Enabling clang-tidy linter")
    set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_BIN})
    set(CMAKE_VS_GLOBALS
      "RunCodeAnalysis=true"
      "EnableClangTidyCodeAnalysis=true"
      "ClangTidyToolExe=${CLANG_TIDY_BIN}")
  else()
    message(FATAL_ERROR "Linting was enabled but the clang-tidy linter could not be found")
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

if(BUILD_FUZZING)
  add_subdirectory(fuzz)
endif()

add_subdirectory(std)
add_subdirectory(utilities)
