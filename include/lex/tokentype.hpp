#pragma once
#include <iostream>

namespace lex {

enum class TokenType {
  OpPlus,
  OpMinus,
  OpStar,
  OpSlash,
  OpQMark,
  OpColon,
  SepOpenParan,
  SepCloseParan,
  SepComma,
  LitInt,
  LitBool,
  LitStr,
  Keyword,
  Identifier,
  Error,
  End,
};

std::ostream& operator<<(std::ostream& out, const TokenType& rhs);

} // namespace lex
