#include "catch2/catch.hpp"
#include "lex/token_cat.hpp"

namespace lex {

TEST_CASE("Retreiving token categories", "[lexer]") {

  SECTION("Every token-type has a category") {
    const int typeBegin = static_cast<int>(TokenType::OpPlus);
    const int typeEnd = static_cast<int>(TokenType::End);

    for (int typeInt = typeBegin; typeInt != typeEnd; typeInt++) {
      const auto type = static_cast<TokenType>(typeInt);
      REQUIRE(lookupCat(type) != TokenCat::Unknown);
    }
  }
}

} // namespace lex
