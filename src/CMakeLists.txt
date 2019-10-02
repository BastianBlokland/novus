# Lexer.
add_library(lex STATIC
  lex/lexer.cpp
  lex/sourcespan.cpp
  lex/token.cpp
  lex/tokentype.cpp
  lex/keyword.cpp)
target_include_directories(lex PUBLIC ../include/)
target_compile_features(lex PUBLIC cxx_std_17)
