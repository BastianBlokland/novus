# Configure fuzz executables using llvm libfuzzer (https://llvm.org/docs/LibFuzzer.html).

if(NOT ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
  message(FATAL_ERROR "Fuzzing is only supported with the clang compiler")
endif()

# 'novfuzz-lex' executable.
message(STATUS "Configuring novfuzz-lex executable")
add_executable(novfuzz-lex fuzz_lex.cpp)
target_compile_features(novfuzz-lex PUBLIC cxx_std_17)
target_compile_options(novfuzz-lex PUBLIC -g -fexceptions -fsanitize=fuzzer)
target_link_options(novfuzz-lex PUBLIC -fsanitize=fuzzer)
target_link_libraries(novfuzz-lex PRIVATE lex)

# 'novfuzz-parse' executable.
message(STATUS "Configuring novfuzz-parse executable")
add_executable(novfuzz-parse fuzz_parse.cpp)
target_compile_features(novfuzz-parse PUBLIC cxx_std_17)
target_compile_options(novfuzz-parse PUBLIC -g -fexceptions -fsanitize=fuzzer)
target_link_options(novfuzz-parse PUBLIC -fsanitize=fuzzer)
target_link_libraries(novfuzz-parse PRIVATE parse)
