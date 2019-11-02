#include "catch2/catch.hpp"
#include "frontend/analysis.hpp"
#include <string>

namespace frontend {

TEST_CASE("Analysis", "[frontend]") {

  SECTION("Parse errors") {
    const auto input  = std::string{"print(1 + .)"};
    const auto src    = buildSource("input", input.begin(), input.end());
    const auto output = analyze(src);
    CHECK(!output.isSuccess());
    for (auto diagItr = output.beginDiags(); diagItr != output.endDiags(); ++diagItr) {
      CHECK(*diagItr == error(src, "Invalid character '.'", input::Span{10, 10}));
    }
  }
}

} // namespace frontend
