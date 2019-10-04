#include "lex/lexer.hpp"
#include <iostream>
#include <string>

auto main() -> int {

  std::istreambuf_iterator<char> eos;
  std::istreambuf_iterator<char> iit{std::cin.rdbuf()};

  auto lexer = lex::Lexer{iit, eos};
  for (const auto& token : lexer) {
    std::cout << '[' << token << ']' << ' ';
  }

  return 0;
}
