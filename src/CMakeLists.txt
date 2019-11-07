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
  parse/node_stmt_exec.cpp
  parse/node_stmt_func_decl.cpp
  parse/node_visitor_deep.cpp
  parse/node.cpp
  parse/op_precedence.cpp
  parse/parser.cpp
  parse/utilities.cpp)
target_compile_features(parse PUBLIC cxx_std_17)
target_link_libraries(parse PUBLIC lex)
target_compile_options(parse PRIVATE -Werror -Wall -Wextra)
target_include_directories(parse PUBLIC ../include)
target_include_directories(parse PRIVATE parse)

# Program (internal representation).
add_library(prog STATIC
  prog/expr/node_assign.cpp
  prog/expr/node_switch.cpp
  prog/expr/node_call.cpp
  prog/expr/node_const.cpp
  prog/expr/node_group.cpp
  prog/expr/node_lit_bool.cpp
  prog/expr/node_lit_int.cpp
  prog/expr/node.cpp
  prog/expr/utilities.cpp

  prog/sym/action_decl_table.cpp
  prog/sym/action_decl.cpp
  prog/sym/action_id.cpp
  prog/sym/action_kind.cpp
  prog/sym/const_decl_table.cpp
  prog/sym/const_decl.cpp
  prog/sym/const_id.cpp
  prog/sym/const_kind.cpp
  prog/sym/exec_def.cpp
  prog/sym/func_decl_table.cpp
  prog/sym/func_decl.cpp
  prog/sym/func_def_table.cpp
  prog/sym/func_def.cpp
  prog/sym/func_id.cpp
  prog/sym/func_kind.cpp
  prog/sym/func_sig.cpp
  prog/sym/input.cpp
  prog/sym/type_decl_table.cpp
  prog/sym/type_decl.cpp
  prog/sym/type_id.cpp
  prog/sym/type_kind.cpp

  prog/intrinsics.cpp
  prog/operator.cpp
  prog/program.cpp)
target_compile_features(prog PUBLIC cxx_std_17)
target_compile_options(prog PRIVATE -Werror -Wall -Wextra)
target_include_directories(prog PUBLIC ../include)
target_include_directories(prog PRIVATE prog)

# Frontend.
add_library(frontend STATIC
  frontend/internal/declare_user_funcs.cpp
  frontend/internal/define_user_funcs.cpp
  frontend/internal/get_expr.cpp
  frontend/internal/get_parse_diags.cpp

  frontend/analysis.cpp
  frontend/diag_defs.cpp
  frontend/diag_severity.cpp
  frontend/diag.cpp
  frontend/output.cpp
  frontend/source.cpp)
target_compile_features(frontend PUBLIC cxx_std_17)
target_link_libraries(frontend PUBLIC parse)
target_link_libraries(frontend PUBLIC prog)
target_compile_options(frontend PRIVATE -Werror -Wall -Wextra)
target_include_directories(frontend PUBLIC ../include)
target_include_directories(frontend PRIVATE frontend)
