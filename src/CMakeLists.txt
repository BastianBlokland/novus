# Lexer.
add_library(lex STATIC
  ../include/lex/error.hpp
  ../include/lex/keyword.hpp
  ../include/lex/lexer.hpp
  ../include/lex/source_span.hpp
  ../include/lex/token.hpp
  ../include/lex/token_cat.hpp
  ../include/lex/token_itr.hpp
  ../include/lex/token_type.hpp
  ../include/lex/utilities.hpp
  lex/char_escape.hpp
  lex/char_escape.cpp
  lex/error.cpp
  lex/lexer.cpp
  lex/source_span.cpp
  lex/token.cpp
  lex/token_cat.cpp
  lex/token_type.cpp
  lex/keyword.cpp
  lex/utilities.cpp)
target_include_directories(lex PUBLIC ../include)
target_include_directories(lex PRIVATE lex)
target_compile_features(lex PUBLIC cxx_std_17)
