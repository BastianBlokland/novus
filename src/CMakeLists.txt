# Input.
message(STATUS "Configuring input library")
add_library(input STATIC
  input/char_escape.cpp
  input/info.cpp
  input/search_paths.cpp
  input/span.cpp
  input/textpos.cpp)
target_compile_features(input PRIVATE cxx_std_17)
if(MSVC)
  target_compile_options(input PRIVATE /EHsc)
else()
  target_compile_options(input PRIVATE -fexceptions)
endif()
target_include_directories(input PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(input PRIVATE input)

# Lexer.
message(STATUS "Configuring lex library")
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
target_compile_features(lex PRIVATE cxx_std_17)
if(MSVC)
  target_compile_options(lex PRIVATE /EHsc)
else()
  target_compile_options(lex PRIVATE -fexceptions)
endif()
target_link_libraries(lex PUBLIC input)
target_include_directories(lex PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(lex PRIVATE lex)

# Parser.
message(STATUS "Configuring parse library")
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
  parse/node_expr_intrinsic.cpp
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
  parse/node_stmt_import.cpp
  parse/node_stmt_struct_decl.cpp
  parse/node_stmt_union_decl.cpp
  parse/node_visitor_deep.cpp
  parse/node.cpp
  parse/op_precedence.cpp
  parse/parser.cpp
  parse/ret_type_spec.cpp
  parse/type_param_list.cpp
  parse/type_substitution_list.cpp
  parse/type.cpp
  parse/utilities.cpp)
target_compile_features(parse PRIVATE cxx_std_17)
if(MSVC)
  target_compile_options(parse PRIVATE /EHsc)
else()
  target_compile_options(parse PRIVATE -fexceptions)
endif()
target_link_libraries(parse PUBLIC lex)
target_include_directories(parse PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(parse PRIVATE parse)

# Program (internal representation).
message(STATUS "Configuring prog library")
add_library(prog STATIC
  prog/expr/node_assign.cpp
  prog/expr/node_call_dyn.cpp
  prog/expr/node_call_self.cpp
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
  prog/expr/node_lit_long.cpp
  prog/expr/node_lit_string.cpp
  prog/expr/node_lit_char.cpp
  prog/expr/node_switch.cpp
  prog/expr/node_union_check.cpp
  prog/expr/node_union_get.cpp
  prog/expr/node_visitor_deep.cpp
  prog/expr/node.cpp
  prog/expr/utilities.cpp

  prog/internal/implicit_conv.cpp
  prog/internal/overload.cpp

  prog/sym/const_decl_table.cpp
  prog/sym/const_decl.cpp
  prog/sym/const_id_hasher.cpp
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
  prog/sym/future_def.cpp
  prog/sym/lazy_def.cpp
  prog/sym/source_id_hasher.cpp
  prog/sym/source_id.cpp
  prog/sym/struct_def.cpp
  prog/sym/type_decl_table.cpp
  prog/sym/type_decl.cpp
  prog/sym/type_def_table.cpp
  prog/sym/type_id_hasher.cpp
  prog/sym/type_id.cpp
  prog/sym/type_kind.cpp
  prog/sym/type_set.cpp
  prog/sym/union_def.cpp

  prog/copy.cpp
  prog/operator.cpp
  prog/program.cpp)
target_compile_features(prog PRIVATE cxx_std_17)
if(MSVC)
  target_compile_options(prog PRIVATE /EHsc)
else()
  target_compile_options(prog PRIVATE -fexceptions)
endif()
target_include_directories(prog PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(prog PRIVATE prog)

# Frontend.
message(STATUS "Configuring frontend library")
add_library(frontend STATIC
  frontend/internal/check_inf_recursion.cpp
  frontend/internal/check_union_exhaustiveness.cpp
  frontend/internal/const_binder.cpp
  frontend/internal/context.cpp
  frontend/internal/declare_user_funcs.cpp
  frontend/internal/declare_user_types.cpp
  frontend/internal/define_exec_stmts.cpp
  frontend/internal/define_user_funcs.cpp
  frontend/internal/define_user_types.cpp
  frontend/internal/delegate_table.cpp
  frontend/internal/fail_table.cpp
  frontend/internal/func_template_inst.cpp
  frontend/internal/func_template_table.cpp
  frontend/internal/func_template.cpp
  frontend/internal/future_table.cpp
  frontend/internal/get_expr.cpp
  frontend/internal/import_sources.cpp
  frontend/internal/intrinsic_call_hooks.cpp
  frontend/internal/lazy_table.cpp
  frontend/internal/patch_args.cpp
  frontend/internal/reflect.cpp
  frontend/internal/source_table_builder.cpp
  frontend/internal/staticint_table.cpp
  frontend/internal/struct_template.cpp
  frontend/internal/type_template_base.cpp
  frontend/internal/type_template_table.cpp
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
  frontend/source_table.cpp
  frontend/source.cpp)
target_compile_features(frontend PRIVATE cxx_std_17)
if(MSVC)
  target_compile_options(frontend PRIVATE /EHsc)
else()
  target_compile_options(frontend PRIVATE -fexceptions)
endif()
target_link_libraries(frontend PUBLIC parse)
target_link_libraries(frontend PUBLIC prog)
target_include_directories(frontend PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(frontend PRIVATE frontend)

# Opt (optimiser).
message(STATUS "Configuring opt library")
add_library(opt STATIC
  opt/internal/const_remapper.cpp
  opt/internal/expr_matchers.cpp
  opt/internal/find_called_funcs.cpp
  opt/internal/find_used_consts.cpp
  opt/internal/find_used_funcs.cpp
  opt/internal/find_used_types.cpp
  opt/internal/intrinsics.cpp
  opt/internal/prog_rewrite.cpp
  opt/internal/utilities.cpp
  opt/call_inline.cpp
  opt/const_elimination.cpp
  opt/optimize.cpp
  opt/precompute_literals.cpp
  opt/treeshake.cpp)
target_compile_features(opt PRIVATE cxx_std_17)
if(MSVC)
  target_compile_options(opt PRIVATE /EHsc)
else()
  target_compile_options(opt PRIVATE -fexceptions)
endif()
target_link_libraries(opt PUBLIC prog)
target_include_directories(opt PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(opt PRIVATE opt)

# Backend (assembly generator).
message(STATUS "Configuring backend library")
add_library(backend STATIC
  backend/internal/gen_expr.cpp
  backend/internal/gen_lazy.cpp
  backend/internal/gen_type_eq.cpp
  backend/internal/utilities.cpp
  backend/generator.cpp)
target_compile_features(backend PRIVATE cxx_std_17)
if(MSVC)
  target_compile_options(backend PRIVATE /EHsc)
else()
  target_compile_options(backend PRIVATE -fexceptions)
endif()
target_link_libraries(backend PUBLIC prog)
target_link_libraries(backend PUBLIC novasm)
target_include_directories(backend PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(backend PRIVATE backend)

# Novus Assembly.
message(STATUS "Configuring novasm library")
add_library(novasm STATIC
  novasm/assembler.cpp
  novasm/disassembler.cpp
  novasm/executable.cpp
  novasm/op_code.cpp
  novasm/pcall_code.cpp
  novasm/serialization.cpp)
target_compile_features(novasm PRIVATE cxx_std_17)
if(MSVC)
  target_compile_options(novasm PRIVATE /EHsc)
else()
  target_compile_options(novasm PRIVATE -fexceptions)
endif()
target_include_directories(novasm PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(novasm PRIVATE novasm)

# Virtual Machine.
message(STATUS "Configuring vm library")
add_library(vm STATIC
  vm/internal/executor_registry.cpp
  vm/internal/executor.cpp
  vm/internal/garbage_collector.cpp
  vm/internal/interupt.cpp
  vm/internal/memory_allocator.cpp
  vm/internal/path_utilities.cpp
  vm/internal/platform_utilities.cpp
  vm/internal/ref_allocator.cpp
  vm/internal/ref.cpp
  vm/internal/thread.cpp
  vm/file.cpp
  vm/platform_interface.cpp
  vm/vm.cpp
  vm/exec_state.cpp)
target_compile_features(vm PRIVATE cxx_std_17)
if(MSVC)
  target_compile_options(vm PRIVATE /GR-)
else()
  target_compile_options(vm PRIVATE -fno-exceptions -fno-rtti)
endif()
target_link_libraries(vm PUBLIC Threads::Threads)
target_link_libraries(vm PUBLIC novasm)
target_include_directories(vm PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(vm PRIVATE vm)
