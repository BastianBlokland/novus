#pragma once
#include <iostream>

namespace lex {

enum class TokenKind {
  End,
  OpPlus,
  OpMinus,
  OpStar,
  OpSlash,
  OpRem,
  OpAmp,
  OpAmpAmp,
  OpPipe,
  OpPipePipe,
  OpEq,
  OpEqEq,
  OpBang,
  OpBangEq,
  OpLe,
  OpLeEq,
  OpGt,
  OpGtEq,
  OpSemi,
  OpQMark,
  SepOpenParen,
  SepCloseParen,
  SepComma,
  SepColon,
  SepArrow,
  LitInt,
  LitBool,
  LitString,
  Keyword,
  Identifier,
  Error
};

auto operator<<(std::ostream& out, const TokenKind& rhs) -> std::ostream&;

} // namespace lex
