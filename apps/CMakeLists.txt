include(FetchContent)

# Cli11 command line parser.
message(STATUS "Fetching dependency: cli11")
FetchContent_Declare(
  cli11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
  GIT_TAG v1.9.1
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(cli11)

# Rang library for colored command line output.
message(STATUS "Fetching dependency: rang")
FetchContent_Declare(
  rang
  GIT_REPOSITORY https://github.com/agauniyal/rang.git
  GIT_TAG v3.1.0
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(rang)

# Compiler.
message(STATUS "Configuring novc executable")
add_executable(novc
  novc/compiler.cpp
  novc/deps.cpp
  novc/main.cpp
  novc/utilities.cpp)
target_compile_features(novc PUBLIC cxx_std_17)
if(MSVC)
  target_compile_options(novc PUBLIC /EHsc)
else()
  target_compile_options(novc PUBLIC -fexceptions)
endif()
target_link_libraries(novc PRIVATE CLI11::CLI11)
target_include_directories(novc PRIVATE ${rang_SOURCE_DIR}/include)
target_link_libraries(novc PRIVATE input)
target_link_libraries(novc PRIVATE frontend)
target_link_libraries(novc PRIVATE backend)
target_link_libraries(novc PRIVATE novasm)
target_link_libraries(novc PRIVATE opt)
if(WIN32)
  configure_file(scripts/novus-install.bat "${EXECUTABLE_OUTPUT_PATH}/novus-install.bat" COPYONLY)
  configure_file(scripts/novus-uninstall.bat "${EXECUTABLE_OUTPUT_PATH}/novus-uninstall.bat" COPYONLY)
endif()

# Runtime.
message(STATUS "Configuring novrt executable")
add_executable(novrt
  novrt/win32/env.cpp
  novrt/win32/err.cpp
  novrt/win32/install.cpp
  novrt/win32/reg.cpp
  novrt/win32/regkey.cpp
  novrt/win32/regval.cpp
  novrt/win32/utilities.cpp
  novrt/install.cpp
  novrt/main.cpp)
target_compile_features(novrt PUBLIC cxx_std_17)
if(MSVC)
  target_compile_options(novrt PUBLIC /GR-)
else()
  target_compile_options(novrt PUBLIC -fno-exceptions -fno-rtti)
endif()
target_link_libraries(novrt PRIVATE vm)
target_link_libraries(novrt PRIVATE novasm)

# Lexer diagnostic tool.
message(STATUS "Configuring novdiag-lex executable")
add_executable(novdiag-lex novdiag-lex/main.cpp)
target_compile_features(novdiag-lex PUBLIC cxx_std_17)
if(MSVC)
  target_compile_options(novdiag-lex PUBLIC /EHsc)
else()
  target_compile_options(novdiag-lex PUBLIC -fexceptions)
endif()
target_link_libraries(novdiag-lex PRIVATE CLI11::CLI11)
target_include_directories(novdiag-lex PRIVATE ${rang_SOURCE_DIR}/include)
target_link_libraries(novdiag-lex PRIVATE lex)

# Parser diagnostic tool.
message(STATUS "Configuring novdiag-parse executable")
add_executable(novdiag-parse novdiag-parse/main.cpp)
target_compile_features(novdiag-parse PUBLIC cxx_std_17)
if(MSVC)
  target_compile_options(novdiag-parse PUBLIC /EHsc)
else()
  target_compile_options(novdiag-parse PUBLIC -fexceptions)
endif()
target_link_libraries(novdiag-parse PRIVATE CLI11::CLI11)
target_include_directories(novdiag-parse PRIVATE ${rang_SOURCE_DIR}/include)
target_link_libraries(novdiag-parse PRIVATE parse)

# Program diagnostic tool.
message(STATUS "Configuring novdiag-prog executable")
add_executable(novdiag-prog novdiag-prog/main.cpp)
target_compile_features(novdiag-prog PUBLIC cxx_std_17)
if(MSVC)
  target_compile_options(novdiag-prog PUBLIC /EHsc)
else()
  target_compile_options(novdiag-prog PUBLIC -fexceptions)
endif()
target_link_libraries(novdiag-prog PRIVATE CLI11::CLI11)
target_include_directories(novdiag-prog PRIVATE ${rang_SOURCE_DIR}/include)
target_link_libraries(novdiag-prog PRIVATE input)
target_link_libraries(novdiag-prog PRIVATE frontend)
target_link_libraries(novdiag-prog PRIVATE opt)

# Assembly diagnostic tool.
message(STATUS "Configuring novdiag-asm executable")
add_executable(novdiag-asm novdiag-asm/main.cpp)
target_compile_features(novdiag-asm PUBLIC cxx_std_17)
if(MSVC)
  target_compile_options(novdiag-asm PUBLIC /EHsc)
else()
  target_compile_options(novdiag-asm PUBLIC -fexceptions)
endif()
target_link_libraries(novdiag-asm PRIVATE CLI11::CLI11)
target_include_directories(novdiag-asm PRIVATE ${rang_SOURCE_DIR}/include)
target_link_libraries(novdiag-asm PRIVATE input)
target_link_libraries(novdiag-asm PRIVATE frontend)
target_link_libraries(novdiag-asm PRIVATE backend)
target_link_libraries(novdiag-asm PRIVATE novasm)
target_link_libraries(novdiag-asm PRIVATE opt)
