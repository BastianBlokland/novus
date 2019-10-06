include(FetchContent)

# 'Catch2' testing framework.
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.9.2)
FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(Catch)

# 'tests' executable.
add_executable(tests
  main.cpp
  lex/helpers.hpp
  lex/identifier_test.cpp
  lex/iterator_test.cpp
  lex/keyword_test.cpp
  lex/litint_test.cpp
  lex/litbool_test.cpp
  lex/litstr_test.cpp
  lex/operators_test.cpp
  lex/seperators_test.cpp)
target_include_directories(tests PRIVATE lex)
target_link_libraries(tests PRIVATE Catch2::Catch2)
target_link_libraries(tests PRIVATE lex)
target_compile_features(tests PRIVATE cxx_std_17)

# Register tests to CTest.
catch_discover_tests(tests)
