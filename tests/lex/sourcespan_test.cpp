#include "catch2/catch.hpp"
#include "lex/lexer.hpp"
#include "lex/source_span.hpp"
#include "lex/utilities.hpp"
#include <vector>

namespace lex {

TEST_CASE("SourceSpan utilities", "[lex]") {

  SECTION("Combine spans") {
    // NOLINTNEXTLINE: Magic numbers
    auto spans = {SourceSpan{23, 34}, SourceSpan{5, 10}, SourceSpan{42, 64}, SourceSpan{37, 40}};
    CHECK(SourceSpan::combine(spans.begin(), spans.end()) == SourceSpan{5, 64});

    CHECK(SourceSpan::combine(SourceSpan{15, 64}, SourceSpan{4, 9}) == SourceSpan{4, 64});
  }
}

} // namespace lex
