# Input.
add_library(input STATIC
  input/info.cpp
  input/span.cpp
  input/textpos.cpp)
target_compile_features(input PUBLIC cxx_std_17)
target_compile_options(input PRIVATE -Werror -Wall -Wextra)
target_include_directories(input PUBLIC ../include)
target_include_directories(input PRIVATE input)

# Lexer.
add_library(lex STATIC
  lex/char_escape.cpp
  lex/error.cpp
  lex/lexer.cpp
  lex/token.cpp
  lex/token_cat.cpp
  lex/token_kind.cpp
  lex/token_payload_lit_string.cpp
  lex/keyword.cpp
  lex/utilities.cpp)
target_compile_features(lex PUBLIC cxx_std_17)
target_link_libraries(lex PUBLIC input)
target_compile_options(lex PRIVATE -Werror -Wall -Wextra)
target_include_directories(lex PUBLIC ../include)
target_include_directories(lex PRIVATE lex)

# Parser.
add_library(parse STATIC
  parse/error.cpp
  parse/node_error.cpp
  parse/node_expr_binary.cpp
  parse/node_expr_call.cpp
  parse/node_expr_const_decl.cpp
  parse/node_expr_const.cpp
  parse/node_expr_group.cpp
  parse/node_expr_lit.cpp
  parse/node_expr_paren.cpp
  parse/node_expr_switch_if.cpp
  parse/node_expr_switch_else.cpp
  parse/node_expr_switch.cpp
  parse/node_expr_unary.cpp
  parse/node_kind.cpp
  parse/node_stmt_exec.cpp
  parse/node_stmt_func_decl.cpp
  parse/node_visitor_recursive.cpp
  parse/node.cpp
  parse/op_precedence.cpp
  parse/parser.cpp
  parse/utilities.cpp)
target_compile_features(parse PUBLIC cxx_std_17)
target_link_libraries(parse PUBLIC lex)
target_compile_options(parse PRIVATE -Werror -Wall -Wextra)
target_include_directories(parse PUBLIC ../include)
target_include_directories(parse PRIVATE parse)

# Frontend.
add_library(frontend STATIC
  frontend/analysis.cpp
  frontend/diag_severity.cpp
  frontend/diag.cpp
  frontend/output.cpp
  frontend/source.cpp)
target_compile_features(frontend PUBLIC cxx_std_17)
target_link_libraries(frontend PUBLIC parse)
target_compile_options(frontend PRIVATE -Werror -Wall -Wextra)
target_include_directories(frontend PUBLIC ../include)
target_include_directories(frontend PRIVATE frontend)
