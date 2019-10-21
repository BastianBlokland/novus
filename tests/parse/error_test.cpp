#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "lex/error.hpp"
#include "parse/error.hpp"

namespace parse {

TEST_CASE("Parsing errors", "[parse]") {

  CHECK_EXPR("123a", errLexError(lex::errLitIntInvalidChar()));

  SECTION("Spans") {
    CHECK_EXPR_SPAN("123a", lex::SourceSpan(0, 3));
    CHECK_EXPR_SPAN("(1 == 2", lex::SourceSpan(0, 6));
  }
}

} // namespace parse
