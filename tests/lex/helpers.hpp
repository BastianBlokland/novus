#pragma once
#include "catch2/catch.hpp"
#include "lex/lexer.hpp"
#include "lex/token.hpp"
#include <array>
#include <string>

namespace lex {

#define REQUIRE_TOKENS(INPUT, ...)                                                                 \
  {                                                                                                \
    std::string input   = INPUT;                                                                   \
    auto lexer          = lex::Lexer{input.begin(), input.end()};                                  \
    auto expectedTokens = {__VA_ARGS__};                                                           \
    for (const auto& expectedToken : expectedTokens) {                                             \
      REQUIRE(lexer.next() == expectedToken);                                                      \
    }                                                                                              \
    REQUIRE(lexer.next() == endToken());                                                           \
  }

#define REQUIRE_SPANS(INPUT, ...)                                                                  \
  {                                                                                                \
    std::string input  = INPUT;                                                                    \
    auto lexer         = lex::Lexer{input.begin(), input.end()};                                   \
    auto expectedSpans = {__VA_ARGS__};                                                            \
    for (const auto& expectedSpan : expectedSpans) {                                               \
      REQUIRE(lexer.next().getSpan() == expectedSpan);                                             \
    }                                                                                              \
  }

} // namespace lex
