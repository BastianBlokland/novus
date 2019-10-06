#include "catch2/catch.hpp"
#include "helpers.hpp"
#include <string>

namespace lex {

TEST_CASE("Lexing keywords", "[lexer]") { REQUIRE_TOKENS("fun", keywordToken(Keyword::Function)); }

} // namespace lex
