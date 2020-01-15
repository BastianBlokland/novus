# Input.
add_library(input STATIC
  input/char_escape.cpp
  input/info.cpp
  input/span.cpp
  input/textpos.cpp)
target_compile_features(input PUBLIC cxx_std_17)
target_compile_options(input PRIVATE -Werror -Wall -Wextra)
target_include_directories(input PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(input PRIVATE input)

# Lexer.
add_library(lex STATIC
  lex/error.cpp
  lex/lexer.cpp
  lex/token.cpp
  lex/token_cat.cpp
  lex/token_kind.cpp
  lex/token_payload_lit_char.cpp
  lex/token_payload_lit_string.cpp
  lex/keyword.cpp
  lex/utilities.cpp)
target_compile_features(lex PUBLIC cxx_std_17)
target_link_libraries(lex PUBLIC input)
target_compile_options(lex PRIVATE -Werror -Wall -Wextra)
target_include_directories(lex PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(lex PRIVATE lex)

# Parser.
add_library(parse STATIC
  parse/argument_list_decl.cpp
  parse/error.cpp
  parse/node_comment.cpp
  parse/node_error.cpp
  parse/node_expr_anon_func.cpp
  parse/node_expr_binary.cpp
  parse/node_expr_call.cpp
  parse/node_expr_conditional.cpp
  parse/node_expr_const_decl.cpp
  parse/node_expr_field.cpp
  parse/node_expr_group.cpp
  parse/node_expr_id.cpp
  parse/node_expr_index.cpp
  parse/node_expr_is.cpp
  parse/node_expr_lit.cpp
  parse/node_expr_paren.cpp
  parse/node_expr_switch_if.cpp
  parse/node_expr_switch_else.cpp
  parse/node_expr_switch.cpp
  parse/node_expr_unary.cpp
  parse/node_stmt_enum_decl.cpp
  parse/node_stmt_exec.cpp
  parse/node_stmt_func_decl.cpp
  parse/node_stmt_struct_decl.cpp
  parse/node_stmt_union_decl.cpp
  parse/node_visitor_deep.cpp
  parse/node.cpp
  parse/op_precedence.cpp
  parse/parser.cpp
  parse/type_param_list.cpp
  parse/type_substitution_list.cpp
  parse/type.cpp
  parse/utilities.cpp)
target_compile_features(parse PUBLIC cxx_std_17)
target_link_libraries(parse PUBLIC lex)
target_compile_options(parse PRIVATE -Werror -Wall -Wextra)
target_include_directories(parse PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(parse PRIVATE parse)

# Program (internal representation).
add_library(prog STATIC
  prog/expr/node_assign.cpp
  prog/expr/node_call_dyn.cpp
  prog/expr/node_call.cpp
  prog/expr/node_closure.cpp
  prog/expr/node_const.cpp
  prog/expr/node_fail.cpp
  prog/expr/node_field.cpp
  prog/expr/node_group.cpp
  prog/expr/node_lit_bool.cpp
  prog/expr/node_lit_enum.cpp
  prog/expr/node_lit_float.cpp
  prog/expr/node_lit_func.cpp
  prog/expr/node_lit_int.cpp
  prog/expr/node_lit_string.cpp
  prog/expr/node_lit_char.cpp
  prog/expr/node_switch.cpp
  prog/expr/node_union_check.cpp
  prog/expr/node_union_get.cpp
  prog/expr/node.cpp
  prog/expr/utilities.cpp

  prog/internal/conversion.cpp
  prog/internal/overload.cpp

  prog/sym/action_decl_table.cpp
  prog/sym/action_decl.cpp
  prog/sym/action_id.cpp
  prog/sym/const_decl_table.cpp
  prog/sym/const_decl.cpp
  prog/sym/const_id.cpp
  prog/sym/const_kind.cpp
  prog/sym/delegate_def.cpp
  prog/sym/enum_def.cpp
  prog/sym/exec_stmt.cpp
  prog/sym/field_decl_table.cpp
  prog/sym/field_decl.cpp
  prog/sym/field_id.cpp
  prog/sym/func_decl_table.cpp
  prog/sym/func_decl.cpp
  prog/sym/func_def_table.cpp
  prog/sym/func_def.cpp
  prog/sym/func_id_hasher.cpp
  prog/sym/func_id.cpp
  prog/sym/struct_def.cpp
  prog/sym/type_decl_table.cpp
  prog/sym/type_decl.cpp
  prog/sym/type_def_table.cpp
  prog/sym/type_id_hasher.cpp
  prog/sym/type_id.cpp
  prog/sym/type_kind.cpp
  prog/sym/type_set.cpp
  prog/sym/union_def.cpp

  prog/operator.cpp
  prog/program.cpp)
target_compile_features(prog PUBLIC cxx_std_17)
target_compile_options(prog PRIVATE -Werror -Wall -Wextra)
target_include_directories(prog PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(prog PRIVATE prog)

# Frontend.
add_library(frontend STATIC
  frontend/internal/check_union_exhaustiveness.cpp
  frontend/internal/const_binder.cpp
  frontend/internal/context.cpp
  frontend/internal/declare_user_funcs.cpp
  frontend/internal/declare_user_types.cpp
  frontend/internal/define_exec_stmts.cpp
  frontend/internal/define_user_funcs.cpp
  frontend/internal/define_user_types.cpp
  frontend/internal/get_expr.cpp
  frontend/internal/get_identifier.cpp
  frontend/internal/get_parse_diags.cpp
  frontend/internal/struct_template.cpp
  frontend/internal/type_info.cpp
  frontend/internal/type_template_base.cpp
  frontend/internal/type_template_inst.cpp
  frontend/internal/type_template_table.cpp
  frontend/internal/delegate_table.cpp
  frontend/internal/func_template_inst.cpp
  frontend/internal/func_template_table.cpp
  frontend/internal/func_template.cpp
  frontend/internal/type_substitution_table.cpp
  frontend/internal/typeinfer_expr.cpp
  frontend/internal/typeinfer_typesub.cpp
  frontend/internal/typeinfer_user_funcs.cpp
  frontend/internal/union_template.cpp
  frontend/internal/utilities.cpp
  frontend/internal/validate_types.cpp

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
target_include_directories(frontend PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(frontend PRIVATE frontend)

# Backend (assembly generator).
add_library(backend STATIC
  backend/dasm/disassembler.cpp
  backend/dasm/instruction.cpp
  backend/internal/gen_expr.cpp
  backend/internal/gen_type_eq.cpp
  backend/internal/utilities.cpp
  backend/builder.cpp
  backend/generator.cpp)
target_compile_features(backend PUBLIC cxx_std_17)
target_link_libraries(backend PUBLIC prog)
target_link_libraries(backend PUBLIC vm)
target_compile_options(backend PRIVATE -Werror -Wall -Wextra)
target_include_directories(backend PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(backend PRIVATE backend)

# Virtual Machine.
add_library(vm STATIC
  vm/exceptions/call_stack_overflow.cpp
  vm/exceptions/const_stack_overflow.cpp
  vm/exceptions/div_by_zero.cpp
  vm/exceptions/eval_stack_not_empty.cpp
  vm/exceptions/eval_stack_overflow.cpp
  vm/exceptions/invalid_assembly.cpp
  vm/internal/allocator.cpp
  vm/internal/call_stack.cpp
  vm/internal/const_stack.cpp
  vm/internal/exec_scope.cpp
  vm/internal/string_utilities.cpp
  vm/io/memory_interface.cpp
  vm/io/terminal_interface.cpp
  vm/assembly.cpp
  vm/executor.cpp
  vm/opcode.cpp)
target_compile_features(vm PUBLIC cxx_std_17)
target_compile_options(vm PRIVATE -Werror -Wall -Wextra)
target_include_directories(vm PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(vm PRIVATE vm)
