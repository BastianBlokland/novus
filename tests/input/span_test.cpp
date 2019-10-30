#include "catch2/catch.hpp"
#include "input/span.hpp"
#include <vector>

namespace input {

TEST_CASE("Span utilities", "[input]") {

  SECTION("Combine spans") {
    // NOLINTNEXTLINE: Magic numbers
    auto spans = {Span{23, 34}, Span{5, 10}, Span{42, 64}, Span{37, 40}};
    CHECK(Span::combine(spans.begin(), spans.end()) == Span{5, 64});

    CHECK(Span::combine(Span{15, 64}, Span{4, 9}) == Span{4, 64});
  }
}

} // namespace input
