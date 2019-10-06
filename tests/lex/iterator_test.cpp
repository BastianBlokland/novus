#include "catch2/catch.hpp"
#include "lex/lexer.hpp"
#include <string>

namespace lex {

TEST_CASE("Iterating the lexer", "[lexer]") {
  std::string input = "x + y / z";
  auto lexer = Lexer{input.begin(), input.end()};

  SECTION("Range for") {
    auto i = 0;
    for (const auto& token : lexer) {
      ++i;
    }
    REQUIRE(i == 5);
  }
}

} // namespace lex
