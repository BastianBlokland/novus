#include "catch2/catch.hpp"
#include "input/source_span.hpp"
#include <vector>

namespace input {

TEST_CASE("SourceSpan utilities", "[input]") {

  SECTION("Combine spans") {
    // NOLINTNEXTLINE: Magic numbers
    auto spans = {SourceSpan{23, 34}, SourceSpan{5, 10}, SourceSpan{42, 64}, SourceSpan{37, 40}};
    CHECK(SourceSpan::combine(spans.begin(), spans.end()) == SourceSpan{5, 64});

    CHECK(SourceSpan::combine(SourceSpan{15, 64}, SourceSpan{4, 9}) == SourceSpan{4, 64});
  }
}

} // namespace input
