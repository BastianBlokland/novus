include(FetchContent)

# Cli11 command line parser.
FetchContent_Declare(
  cli11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
  GIT_TAG v1.8.0
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(cli11)

# Rang library for colored command line output.
FetchContent_Declare(
  rang
  GIT_REPOSITORY https://github.com/agauniyal/rang.git
  GIT_TAG v3.1.0
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(rang)

# Evaluator.
add_executable(nove nove/main.cpp)
if(MSVC)
  target_compile_options(nove PUBLIC /EHsc)
else()
  target_compile_options(nove PUBLIC -fexceptions)
endif()
target_include_directories(nove PRIVATE ${rang_SOURCE_DIR}/include)
target_link_libraries(nove PRIVATE frontend)
target_link_libraries(nove PRIVATE backend)
target_link_libraries(nove PRIVATE vm)
target_link_libraries(nove PRIVATE opt)

# Lexer diagnostic tool.
add_executable(novdiag-lex novdiag-lex/main.cpp)
if(MSVC)
  target_compile_options(novdiag-lex PUBLIC /EHsc)
else()
  target_compile_options(novdiag-lex PUBLIC -fexceptions)
endif()
target_link_libraries(novdiag-lex PRIVATE CLI11::CLI11)
target_include_directories(novdiag-lex PRIVATE ${rang_SOURCE_DIR}/include)
target_link_libraries(novdiag-lex PRIVATE lex)

# Parser diagnostic tool.
add_executable(novdiag-parse novdiag-parse/main.cpp)
if(MSVC)
  target_compile_options(novdiag-parse PUBLIC /EHsc)
else()
  target_compile_options(novdiag-parse PUBLIC -fexceptions)
endif()
target_link_libraries(novdiag-parse PRIVATE CLI11::CLI11)
target_include_directories(novdiag-parse PRIVATE ${rang_SOURCE_DIR}/include)
target_link_libraries(novdiag-parse PRIVATE parse)

# Program diagnostic tool.
add_executable(novdiag-prog novdiag-prog/main.cpp)
if(MSVC)
  target_compile_options(novdiag-prog PUBLIC /EHsc)
else()
  target_compile_options(novdiag-prog PUBLIC -fexceptions)
endif()
target_link_libraries(novdiag-prog PRIVATE CLI11::CLI11)
target_include_directories(novdiag-prog PRIVATE ${rang_SOURCE_DIR}/include)
target_link_libraries(novdiag-prog PRIVATE frontend)
target_link_libraries(novdiag-prog PRIVATE opt)

# Assembly diagnostic tool.
add_executable(novdiag-asm novdiag-asm/main.cpp)
if(MSVC)
  target_compile_options(novdiag-asm PUBLIC /EHsc)
else()
  target_compile_options(novdiag-asm PUBLIC -fexceptions)
endif()
target_link_libraries(novdiag-asm PRIVATE CLI11::CLI11)
target_include_directories(novdiag-asm PRIVATE ${rang_SOURCE_DIR}/include)
target_link_libraries(novdiag-asm PRIVATE frontend)
target_link_libraries(novdiag-asm PRIVATE backend)
target_link_libraries(novdiag-asm PRIVATE novasm)
target_link_libraries(novdiag-asm PRIVATE opt)
