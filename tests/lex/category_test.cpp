#include "catch2/catch.hpp"
#include "lex/token_cat.hpp"

namespace lex {

TEST_CASE("[lex] Retreiving token categories", "lex") {

  SECTION("Every token-kind has a category") {
    const int kindBegin = static_cast<int>(TokenKind::End) + 1;
    const int kindEnd   = static_cast<int>(TokenKind::Error) + 1;

    for (int kindInt = kindBegin; kindInt != kindEnd; kindInt++) {
      const auto kind = static_cast<TokenKind>(kindInt);
      CHECK(lookupCat(kind) != TokenCat::Unknown);
    }
  }
}

} // namespace lex
