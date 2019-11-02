#include "catch2/catch.hpp"
#include "frontend/source.hpp"
#include <algorithm>
#include <string>

namespace frontend {

TEST_CASE("Source representation", "[frontend]") {

  SECTION("Build") {
    const auto input = std::string{"print(1)\nprint(2)"};
    const auto src   = buildSource("input", input.begin(), input.end());

    CHECK(src.getId() == "input");
    CHECK(src.getTextPos(9) == input::TextPos{2, 1});
    CHECK(std::count_if(src.begin(), src.end(), [](const parse::NodePtr& n) -> bool {
            return n != nullptr;
          }) == 2);
  }
}

} // namespace frontend
