#include "lex/lexer.hpp"
#include <iostream>
#include <string>

auto main() -> int {

  std::istreambuf_iterator<char> eos;
  std::istreambuf_iterator<char> iit(std::cin.rdbuf());

  auto lexer = lex::Lexer{iit, eos};
  lex::Token token;
  do {
    token = lexer.next();
    std::cout << '[' << token << ']' << ' ';
  } while (!token.isEnd());

  return 0;
}
