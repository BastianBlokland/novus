#pragma once
#include <iostream>

namespace lex {

enum class TokenKind {
  End,            // \0
  OpPlus,         // +
  OpPlusPlus,     // ++
  OpMinus,        // -
  OpMinusMinus,   // --
  OpStar,         // *
  OpSlash,        // /
  OpRem,          // %
  OpAmp,          // &
  OpAmpAmp,       // &&
  OpPipe,         // |
  OpPipePipe,     // ||
  OpHat,          // ^
  OpTilde,        // ~
  OpShiftL,       // <<
  OpShiftR,       // >>
  OpEq,           // =
  OpEqEq,         // ==
  OpBang,         // !
  OpBangBang,     // !!
  OpBangEq,       // !=
  OpLe,           // <
  OpLeEq,         // <=
  OpGt,           // >
  OpGtEq,         // >=
  OpSemi,         // ;
  OpQMark,        // ?
  OpQMarkQMark,   // ??
  OpDot,          // .
  OpColonColon,   // ::
  OpSquareSquare, // []
  OpParenParen,   // ()
  SepOpenParen,   // (
  SepCloseParen,  // )
  SepOpenCurly,   // {
  SepCloseCurly,  // }
  SepOpenSquare,  // [
  SepCloseSquare, // ]
  SepComma,       // ,
  SepColon,       // :
  SepArrow,       // ->
  LitInt,         // 42
  LitLong,        // 42L
  LitFloat,       // 4.2
  LitBool,        // true
  LitString,      // "hello world"
  LitChar,        // 'h'
  Keyword,        // if
  Identifier,     // helloworld
  LineComment,    // // Hello world
  Discard,        // _
  Error
};

auto operator<<(std::ostream& out, const TokenKind& rhs) -> std::ostream&;

} // namespace lex
