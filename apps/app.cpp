#include "lex/lexer.hpp"
#include <iostream>
#include <string>

auto main() -> int {
  std::cout << "Type input:\n";

  for (std::string line; std::getline(std::cin, line);) {
    std::cout << '\n';
    auto lexer = lex::Lexer{line.begin(), line.end()};
    lex::Token token;
    do {
      token = lexer.next();
      std::cout << '[' << token << ']' << ' ';
    } while (!token.isEnd());
    std::cout << '\n' << '\n';
  }

  return 0;
}
