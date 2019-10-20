#include "catch2/catch.hpp"
#include "lex/lexer.hpp"
#include "parse/parser.hpp"
#include <string>

namespace parse {

TEST_CASE("Iterating the parser", "[parse]") {
  const std::string input = "print 1 print x * y";
  auto lexer              = lex::Lexer{input.begin(), input.end()};
  auto parser             = parse::Parser{lexer.begin(), lexer.end()};

  SECTION("Range for") {
    std::vector<NodePtr> nodes;
    for (auto&& node : parser) {
      nodes.push_back(std::move(node));
    }
    REQUIRE(nodes.size() == 2);
  }

  SECTION("While loop") {
    auto i = 0;
    while (parser.next() != nullptr) {
      ++i;
    }
    REQUIRE(i == 2);
  }

  SECTION("Iterator") {
    auto i       = 0;
    auto nodeItr = parser.begin();
    auto endItr  = parser.end();
    for (; nodeItr != endItr; ++nodeItr) {
      i++;
    }
    REQUIRE(i == 2);
  }

  SECTION("ParseAll") {
    auto vec = parseAll(lexer.begin(), lexer.end());
    REQUIRE(vec.size() == 2);
  }
}

} // namespace parse
