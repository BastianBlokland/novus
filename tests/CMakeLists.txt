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

  backend/assembly_output.cpp
  backend/call_expr_test.cpp
  backend/constants_test.cpp
  backend/literals_test.cpp
  backend/switch_expr_test.cpp

  frontend/declare_user_funcs_test.cpp
  frontend/define_exec_stmts_test.cpp
  frontend/define_user_funcs_test.cpp
  frontend/get_binary_expr_test.cpp
  frontend/get_call_expr_test.cpp
  frontend/get_conditional_expr_test.cpp
  frontend/get_const_expr_test.cpp
  frontend/get_group_expr_test.cpp
  frontend/get_lit_expr_test.cpp
  frontend/get_paren_expr_test.cpp
  frontend/get_switch_expr_test.cpp
  frontend/get_unary_expr_test.cpp
  frontend/overload_test.cpp
  frontend/parse_diags_test.cpp
  frontend/source_test.cpp
  frontend/typeinfer_user_funcs_test.cpp

  input/char_escape_test.cpp
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
  parse/expr_condtional_test.cpp
  parse/expr_group_test.cpp
  parse/expr_paren_test.cpp
  parse/expr_primary_test.cpp
  parse/expr_switch_test.cpp
  parse/expr_unary_test.cpp
  parse/iterator_test.cpp
  parse/stmt_func_decl_test.cpp
  parse/stmt_exec_test.cpp

  vm/call_test.cpp
  vm/consts_test.cpp
  vm/conv_test.cpp
  vm/error_test.cpp
  vm/int_check_test.cpp
  vm/int_op_test.cpp
  vm/io_test.cpp
  vm/jump_test.cpp
  vm/literal_test.cpp
  vm/string_check_test.cpp
  vm/string_op_test.cpp)
target_compile_features(tests PRIVATE cxx_std_17)
target_compile_options(tests PRIVATE -Werror -Wall -Wextra)
target_include_directories(tests PRIVATE lex)
target_link_libraries(tests PRIVATE Catch2::Catch2)
target_link_libraries(tests PRIVATE lex)
target_link_libraries(tests PRIVATE parse)
target_link_libraries(tests PRIVATE frontend)
target_link_libraries(tests PRIVATE backend)
target_link_libraries(tests PRIVATE vm)

# Register tests to CTest.
catch_discover_tests(tests)
