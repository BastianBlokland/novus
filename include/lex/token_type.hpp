#pragma once
#include <iostream>

namespace lex {

enum class TokenType {
  End,
  OpPlus,
  OpMinus,
  OpStar,
  OpSlash,
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
  SepArrow,
  LitInt,
  LitBool,
  LitStr,
  Keyword,
  Identifier,
  Error
};

auto operator<<(std::ostream& out, const TokenType& rhs) -> std::ostream&;

} // namespace lex
