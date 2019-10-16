# Lexer.
add_library(lex STATIC
  ../include/lex/error.hpp
  ../include/lex/keyword.hpp
  ../include/lex/lexer.hpp
  ../include/lex/source_span.hpp
  ../include/lex/token.hpp
  ../include/lex/token_cat.hpp
  ../include/lex/token_itr.hpp
  ../include/lex/token_payload.hpp
  ../include/lex/token_type.hpp
  ../include/lex/utilities.hpp
  lex/char_escape.hpp
  lex/char_escape.cpp
  lex/error.cpp
  lex/lexer.cpp
  lex/source_span.cpp
  lex/token.cpp
  lex/token_cat.cpp
  lex/token_payload.cpp
  lex/token_type.cpp
  lex/keyword.cpp
  lex/utilities.cpp)
target_compile_features(lex PUBLIC cxx_std_17)
target_compile_options(lex PRIVATE -Werror -Wall -Wextra)
target_include_directories(lex PUBLIC ../include)
target_include_directories(lex PRIVATE lex)

# Parser.
add_library(parse STATIC
  ../include/parse/error.hpp
  ../include/parse/node_error.hpp
  ../include/parse/node_expr_binary.hpp
  ../include/parse/node_expr_comma.hpp
  ../include/parse/node_expr_paren.hpp
  ../include/parse/node_expr_unary.hpp
  ../include/parse/node_itr.hpp
  ../include/parse/node_stmt_print.hpp
  ../include/parse/node_type.hpp
  ../include/parse/node.hpp
  ../include/parse/parser.hpp
  parse/error.cpp
  parse/node_type.cpp
  parse/node.cpp
  parse/op_precedence.cpp
  parse/op_precedence.hpp
  parse/parser.cpp)
target_compile_features(parse PUBLIC cxx_std_17)
target_link_libraries(parse PUBLIC lex)
target_compile_options(parse PRIVATE -Werror -Wall -Wextra)
target_include_directories(parse PUBLIC ../include)
target_include_directories(parse PRIVATE parse)
