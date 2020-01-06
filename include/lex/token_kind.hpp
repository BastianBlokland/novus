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
  OpHat,
  OpShiftL,
  OpShiftR,
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
  OpDot,
  OpColonColon,
  OpSquareSquare,
  OpParenParen,
  SepOpenParen,
  SepCloseParen,
  SepOpenCurly,
  SepCloseCurly,
  SepOpenSquare,
  SepCloseSquare,
  SepComma,
  SepColon,
  SepArrow,
  LitInt,
  LitFloat,
  LitBool,
  LitString,
  Keyword,
  Identifier,
  LineComment,
  Discard,
  Error
};

auto operator<<(std::ostream& out, const TokenKind& rhs) -> std::ostream&;

} // namespace lex
