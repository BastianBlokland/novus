#include "catch2/catch.hpp"
#include "lex/lexer.hpp"
#include "lex/utilities.hpp"
#include <optional>
#include <string>

namespace lex {

TEST_CASE("Lexer utilities", "[lexer]") {

  SECTION("Find token by position") {
    const std::string input = "\"Hello world\" + 1_000 * true";
    const auto tokens = lexAll(input.begin(), input.end());

    REQUIRE(findToken(tokens.begin(), tokens.end(), -1) == std::nullopt);
    REQUIRE(findToken(tokens.begin(), tokens.end(), 0) == litStrToken("Hello world"));
    REQUIRE(findToken(tokens.begin(), tokens.end(), 5) == litStrToken("Hello world"));
    REQUIRE(findToken(tokens.begin(), tokens.end(), 13) == std::nullopt);
    REQUIRE(findToken(tokens.begin(), tokens.end(), 14) == basicToken(TokenType::OpPlus));
    REQUIRE(findToken(tokens.begin(), tokens.end(), 17) == litIntToken(1000));
    REQUIRE(findToken(tokens.begin(), tokens.end(), 22) == basicToken(TokenType::OpStar));
    REQUIRE(findToken(tokens.begin(), tokens.end(), 24) == litBoolToken(true));
    REQUIRE(findToken(tokens.begin(), tokens.end(), 28) == std::nullopt);
  }
}

} // namespace lex
