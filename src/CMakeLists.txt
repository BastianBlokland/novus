# Lexer.
add_library(lex STATIC
  ../include/lex/error.hpp
  ../include/lex/keyword.hpp
  ../include/lex/lexer.hpp
  ../include/lex/sourcespan.hpp
  ../include/lex/token.hpp
  ../include/lex/token_itr.hpp
  ../include/lex/token_type.hpp
  lex/char_escape.hpp
  lex/char_escape.cpp
  lex/error.cpp
  lex/lexer.cpp
  lex/sourcespan.cpp
  lex/token.cpp
  lex/token_type.cpp
  lex/keyword.cpp)
target_include_directories(lex PUBLIC ../include)
target_include_directories(lex PRIVATE lex)
target_compile_features(lex PUBLIC cxx_std_17)
