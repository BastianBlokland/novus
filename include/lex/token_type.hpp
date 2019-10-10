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
  SepArrow,
  LitInt,
  LitBool,
  LitStr,
  Keyword,
  Identifier,
  Error,
  End,
};

auto operator<<(std::ostream& out, const TokenType& rhs) -> std::ostream&;

} // namespace lex
