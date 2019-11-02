include(FetchContent)

# 'Catch2' testing framework.
FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.9.2
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(Catch)

# 'tests' executable.
add_executable(tests
  main.cpp

  frontend/source_test.cpp

  input/info_test.cpp
  input/span_test.cpp

  lex/category_test.cpp
  lex/identifier_test.cpp
  lex/iterator_test.cpp
  lex/keyword_test.cpp
  lex/litint_test.cpp
  lex/litbool_test.cpp
  lex/litstr_test.cpp
  lex/operators_test.cpp
  lex/seperators_test.cpp
  lex/utilities_test.cpp

  parse/error_test.cpp
  parse/expr_binary_test.cpp
  parse/expr_call_test.cpp
  parse/expr_group_test.cpp
  parse/expr_paren_test.cpp
  parse/expr_primary_test.cpp
  parse/expr_switch_test.cpp
  parse/expr_unary_test.cpp
  parse/iterator_test.cpp
  parse/stmt_func_decl_test.cpp
  parse/stmt_exec_test.cpp)
target_compile_features(tests PRIVATE cxx_std_17)
target_compile_options(tests PRIVATE -Werror -Wall -Wextra)
target_include_directories(tests PRIVATE lex)
target_link_libraries(tests PRIVATE Catch2::Catch2)
target_link_libraries(tests PRIVATE lex)
target_link_libraries(tests PRIVATE parse)
target_link_libraries(tests PRIVATE frontend)

# Register tests to CTest.
catch_discover_tests(tests)
