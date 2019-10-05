# Lexer.
add_library(lex STATIC
  lex/lexer.cpp
  lex/sourcespan.cpp
  lex/token.cpp
  lex/tokentype.cpp
  lex/keyword.cpp
  ../include/lex/error.hpp
  ../include/lex/keyword.hpp
  ../include/lex/lexer.hpp
  ../include/lex/sourcespan.hpp
  ../include/lex/token.hpp
  ../include/lex/tokenitr.hpp
  ../include/lex/tokentype.hpp)
target_include_directories(lex PUBLIC ../include/)
target_compile_features(lex PUBLIC cxx_std_17)
