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
  OpAmp,
  OpAmpAmp,
  OpPipe,
  OpPipePipe,
  OpEq,
  OpEqEq,
  OpBang,
  OpBangEq,
  OpLess,
  OpLessEq,
  OpGt,
  OpGtEq,
  SepOpenParan,
  SepCloseParan,
  SepComma,
  SepUnderscore,
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
