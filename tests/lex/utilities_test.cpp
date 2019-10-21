#include "catch2/catch.hpp"
#include "lex/lexer.hpp"
#include "lex/utilities.hpp"
#include <string>

namespace lex {

TEST_CASE("Lexer utilities", "[lex]") {

  SECTION("Find token by position") {
    const std::string input = "\"Hello world\" + 1_000 * true";
    const auto tokens       = lexAll(input.begin(), input.end());

    CHECK(findToken(tokens.begin(), tokens.end(), -1) == std::nullopt);
    CHECK(findToken(tokens.begin(), tokens.end(), 0) == litStrToken("Hello world"));
    CHECK(findToken(tokens.begin(), tokens.end(), 5) == litStrToken("Hello world"));
    CHECK(findToken(tokens.begin(), tokens.end(), 13) == std::nullopt);
    CHECK(findToken(tokens.begin(), tokens.end(), 14) == basicToken(TokenType::OpPlus));
    CHECK(findToken(tokens.begin(), tokens.end(), 17) == litIntToken(1000));
    CHECK(findToken(tokens.begin(), tokens.end(), 22) == basicToken(TokenType::OpStar));
    CHECK(findToken(tokens.begin(), tokens.end(), 24) == litBoolToken(true));
    CHECK(findToken(tokens.begin(), tokens.end(), 28) == std::nullopt);
  }
}

} // namespace lex
