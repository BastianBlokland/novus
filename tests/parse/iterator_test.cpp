#include "catch2/catch.hpp"
#include "lex/lexer.hpp"
#include "parse/parser.hpp"
#include <string>

namespace parse {

TEST_CASE("[parse] Iterating the parser", "parse") {
  const std::string input = "print(1) print(x * y)";

  SECTION("Range for") {
    auto lexer  = lex::Lexer{input.begin(), input.end()};
    auto parser = parse::Parser{lexer.begin(), lexer.end()};

    std::vector<NodePtr> nodes;
    for (auto&& node : parser) {
      nodes.push_back(std::move(node));
    }
    REQUIRE(nodes.size() == 2);
  }

  SECTION("While loop") {
    auto lexer  = lex::Lexer{input.begin(), input.end()};
    auto parser = parse::Parser{lexer.begin(), lexer.end()};

    auto i = 0;
    while (parser.next() != nullptr) {
      ++i;
    }
    REQUIRE(i == 2);
  }

  SECTION("Iterator") {
    auto lexer  = lex::Lexer{input.begin(), input.end()};
    auto parser = parse::Parser{lexer.begin(), lexer.end()};

    auto i       = 0;
    auto nodeItr = parser.begin();
    auto endItr  = parser.end();
    for (; nodeItr != endItr; ++nodeItr) {
      i++;
    }
    REQUIRE(i == 2);
  }

  SECTION("ParseAll") {
    auto lexer = lex::Lexer{input.begin(), input.end()};

    auto vec = parseAll(lexer.begin(), lexer.end());
    REQUIRE(vec.size() == 2);
  }
}

} // namespace parse
