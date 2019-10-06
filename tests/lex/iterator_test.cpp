#include "catch2/catch.hpp"
#include "lex/lexer.hpp"
#include "lex/token_itr.hpp"
#include <string>
#include <vector>

namespace lex {

TEST_CASE("Iterating the lexer", "[lexer]") {
  const std::string input = "x + y / z";
  auto lexer = Lexer{input.begin(), input.end()};

  SECTION("Range for") {
    auto i = 0;
    for (const auto& token : lexer) {
      ++i;
    }
    REQUIRE(i == 5);
  }

  SECTION("While loop") {
    auto i = 0;
    Token token;
    while (!(token = lexer.next()).isEnd()) {
      ++i;
    }
    REQUIRE(i == 5);
  }

  SECTION("Iterator") {
    auto i = 0;
    auto tokenItr = TokenItr{lexer};
    auto endItr = TokenItr{};
    for (; tokenItr != endItr; ++tokenItr) {
      i++;
    }
    REQUIRE(i == 5);
  }

  SECTION("To vector") {
    std::vector<Token> vec{lexer.begin(), lexer.end()};
    REQUIRE(vec.size() == 5);
  }

  SECTION("LexAll") {
    auto vec = lexAll(input.begin(), input.end());
    REQUIRE(vec.size() == 5);
  }
}

} // namespace lex
