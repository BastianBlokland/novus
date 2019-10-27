#include "catch2/catch.hpp"
#include "lex/token_cat.hpp"

namespace lex {

TEST_CASE("Retreiving token categories", "[lex]") {

  SECTION("Every token-type has a category") {
    const int typeBegin = static_cast<int>(TokenKind::End) + 1;
    const int typeEnd   = static_cast<int>(TokenKind::Error) + 1;

    for (int typeInt = typeBegin; typeInt != typeEnd; typeInt++) {
      const auto type = static_cast<TokenKind>(typeInt);
      CHECK(lookupCat(type) != TokenCat::Unknown);
    }
  }
}

} // namespace lex
