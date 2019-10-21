#pragma once
#include "catch2/catch.hpp"
#include "lex/lexer.hpp"
#include <string>

namespace lex {

#define CHECK_TOKENS(INPUT, ...)                                                                   \
  {                                                                                                \
    std::string input   = INPUT;                                                                   \
    auto lexer          = lex::Lexer{input.begin(), input.end()};                                  \
    auto expectedTokens = {__VA_ARGS__};                                                           \
    for (const auto& expectedToken : expectedTokens) {                                             \
      CHECK(lexer.next() == expectedToken);                                                        \
    }                                                                                              \
    CHECK(lexer.next() == endToken());                                                             \
  }

#define CHECK_SPANS(INPUT, ...)                                                                    \
  {                                                                                                \
    std::string input  = INPUT;                                                                    \
    auto lexer         = lex::Lexer{input.begin(), input.end()};                                   \
    auto expectedSpans = {__VA_ARGS__};                                                            \
    for (const auto& expectedSpan : expectedSpans) {                                               \
      CHECK(lexer.next().getSpan() == expectedSpan);                                               \
    }                                                                                              \
  }

} // namespace lex
