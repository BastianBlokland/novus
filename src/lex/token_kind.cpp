#include "lex/token_kind.hpp"

namespace lex {

auto operator<<(std::ostream& out, const TokenKind& rhs) -> std::ostream& {
  switch (rhs) {
  case TokenKind::OpPlus:
    out << "+";
    break;
  case TokenKind::OpMinus:
    out << "-";
    break;
  case TokenKind::OpStar:
    out << "*";
    break;
  case TokenKind::OpSlash:
    out << "/";
    break;
  case TokenKind::OpRem:
    out << "%";
    break;
  case TokenKind::OpAmp:
    out << "&";
    break;
  case TokenKind::OpAmpAmp:
    out << "&&";
    break;
  case TokenKind::OpPipe:
    out << "|";
    break;
  case TokenKind::OpPipePipe:
    out << "||";
    break;
  case TokenKind::OpHat:
    out << "^";
    break;
  case TokenKind::OpShiftL:
    out << "<<";
    break;
  case TokenKind::OpShiftR:
    out << ">>";
    break;
  case TokenKind::OpEq:
    out << "=";
    break;
  case TokenKind::OpEqEq:
    out << "==";
    break;
  case TokenKind::OpBang:
    out << "!";
    break;
  case TokenKind::OpBangEq:
    out << "!=";
    break;
  case TokenKind::OpLe:
    out << "<";
    break;
  case TokenKind::OpLeEq:
    out << "<=";
    break;
  case TokenKind::OpGt:
    out << ">";
    break;
  case TokenKind::OpGtEq:
    out << ">=";
    break;
  case TokenKind::OpSemi:
    out << ";";
    break;
  case TokenKind::OpQMark:
    out << "?";
    break;
  case TokenKind::OpQMarkQMark:
    out << "??";
    break;
  case TokenKind::OpDot:
    out << ".";
    break;
  case TokenKind::OpColonColon:
    out << "::";
    break;
  case TokenKind::OpSquareSquare:
    out << "[]";
    break;
  case TokenKind::OpParenParen:
    out << "()";
    break;
  case TokenKind::SepOpenParen:
    out << "(";
    break;
  case TokenKind::SepCloseParen:
    out << ")";
    break;
  case TokenKind::SepOpenCurly:
    out << "{";
    break;
  case TokenKind::SepCloseCurly:
    out << "}";
    break;
  case TokenKind::SepOpenSquare:
    out << "[";
    break;
  case TokenKind::SepCloseSquare:
    out << "]";
    break;
  case TokenKind::SepComma:
    out << ",";
    break;
  case TokenKind::SepColon:
    out << ":";
    break;
  case TokenKind::SepArrow:
    out << "->";
    break;
  case TokenKind::LitInt:
    out << "int";
    break;
  case TokenKind::LitFloat:
    out << "float";
    break;
  case TokenKind::LitBool:
    out << "bool";
    break;
  case TokenKind::LitString:
    out << "string";
    break;
  case TokenKind::LitChar:
    out << "char";
    break;
  case TokenKind::Keyword:
    out << "keyword";
    break;
  case TokenKind::Discard:
    out << "discard";
    break;
  case TokenKind::Identifier:
    out << "identifier";
    break;
  case TokenKind::LineComment:
    out << "line-comment";
    break;
  case TokenKind::Error:
    out << "error";
    break;
  case TokenKind::End:
    out << "end";
    break;
  }
  return out;
}

} // namespace lex
