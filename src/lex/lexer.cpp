#include "lex/lexer.hpp"
#include "input/char_escape.hpp"
#include "lex/error.hpp"
#include <cassert>

namespace lex {

namespace internal {

static auto isTokenSeperator(const char& c) {
  switch (c) {
  case '\0':
  case '\t':
  case '\n':
  case '\r':
  case ' ':
  case '!':
  case '"':
  case '#':
  case '$':
  case '%':
  case '&':
  case '(':
  case ')':
  case '*':
  case '+':
  case ',':
  case '-':
  case '.':
  case '/':
  case ':':
  case ';':
  case '<':
  case '=':
  case '>':
  case '?':
  case '@':
  case '[':
  case '\\':
  case ']':
  case '^':
  case '`':
  case '{':
  case '|':
  case '}':
  case '~':
    return true;
  default:
    return false;
  }
}

static auto isDigit(const char& c) { return c >= '0' && c <= '9'; }

static auto isWordStart(const char& c) {
  const unsigned char utf8Start = 0xC0;

  // Either ascii letter or start of non-ascii utf8 character.
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
      (static_cast<unsigned char>(c) >= utf8Start);
}

static auto isWordContinuation(const char& c) {
  const unsigned char utf8Continuation = 0x80;

  // Either ascii letter or continuation of non-ascii utf8 character.
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
      (static_cast<unsigned char>(c) >= utf8Continuation);
}

auto LexerImpl::next() -> Token {
  while (true) {
    const auto c = consumeChar();
    switch (c) {
    case '\0':
      return endToken(input::Span{m_inputPos >= 0 ? m_inputPos : 0});
    case '+':
      if (peekChar(0) == '+') {
        consumeChar();
        return basicToken(TokenKind::OpPlusPlus, input::Span{m_inputPos - 1, m_inputPos});
      }
      return basicToken(TokenKind::OpPlus, input::Span{m_inputPos});
    case '-':
      if (peekChar(0) == '>') {
        consumeChar();
        return basicToken(TokenKind::SepArrow, input::Span{m_inputPos - 1, m_inputPos});
      }
      if (peekChar(0) == '-') {
        consumeChar();
        return basicToken(TokenKind::OpMinusMinus, input::Span{m_inputPos - 1, m_inputPos});
      }
      return basicToken(TokenKind::OpMinus, input::Span{m_inputPos});
    case '*':
      return basicToken(TokenKind::OpStar, input::Span{m_inputPos});
    case '/':
      if (peekChar(0) == '/') {
        return nextLineComment();
      }
      return basicToken(TokenKind::OpSlash, input::Span{m_inputPos});
    case '%':
      return basicToken(TokenKind::OpRem, input::Span{m_inputPos});
    case '&':
      if (peekChar(0) == '&') {
        consumeChar();
        return basicToken(TokenKind::OpAmpAmp, input::Span{m_inputPos - 1, m_inputPos});
      }
      return basicToken(TokenKind::OpAmp, input::Span{m_inputPos});
    case '|':
      if (peekChar(0) == '|') {
        consumeChar();
        return basicToken(TokenKind::OpPipePipe, input::Span{m_inputPos - 1, m_inputPos});
      }
      return basicToken(TokenKind::OpPipe, input::Span{m_inputPos});
    case '^':
      return basicToken(TokenKind::OpHat, input::Span{m_inputPos});
    case '~':
      return basicToken(TokenKind::OpTilde, input::Span{m_inputPos});
    case '=':
      if (peekChar(0) == '=') {
        consumeChar();
        return basicToken(TokenKind::OpEqEq, input::Span{m_inputPos - 1, m_inputPos});
      }
      return basicToken(TokenKind::OpEq, input::Span{m_inputPos});
    case '!':
      if (peekChar(0) == '!') {
        consumeChar();
        return basicToken(TokenKind::OpBangBang, input::Span{m_inputPos - 1, m_inputPos});
      }
      if (peekChar(0) == '=') {
        consumeChar();
        return basicToken(TokenKind::OpBangEq, input::Span{m_inputPos - 1, m_inputPos});
      }
      return basicToken(TokenKind::OpBang, input::Span{m_inputPos});
    case '<':
      if (peekChar(0) == '=') {
        consumeChar();
        return basicToken(TokenKind::OpLeEq, input::Span{m_inputPos - 1, m_inputPos});
      }
      if (peekChar(0) == '<') {
        consumeChar();
        return basicToken(TokenKind::OpShiftL, input::Span{m_inputPos - 1, m_inputPos});
      }
      return basicToken(TokenKind::OpLe, input::Span{m_inputPos});
    case '>':
      if (peekChar(0) == '=') {
        consumeChar();
        return basicToken(TokenKind::OpGtEq, input::Span{m_inputPos - 1, m_inputPos});
      }
      if (peekChar(0) == '>') {
        consumeChar();
        return basicToken(TokenKind::OpShiftR, input::Span{m_inputPos - 1, m_inputPos});
      }
      return basicToken(TokenKind::OpGt, input::Span{m_inputPos});
    case ';':
      return basicToken(TokenKind::OpSemi, input::Span{m_inputPos});
    case '?':
      if (peekChar(0) == '?') {
        consumeChar();
        return basicToken(TokenKind::OpQMarkQMark, input::Span{m_inputPos - 1, m_inputPos});
      }
      return basicToken(TokenKind::OpQMark, input::Span{m_inputPos});
    case '.':
      if (isDigit(peekChar(0))) {
        return nextLitNumber('.');
      }
      return basicToken(TokenKind::OpDot, input::Span{m_inputPos});
    case '(':
      if (peekChar(0) == ')') {
        consumeChar();
        return basicToken(TokenKind::OpParenParen, input::Span{m_inputPos - 1, m_inputPos});
      }
      return basicToken(TokenKind::SepOpenParen, input::Span{m_inputPos});
    case ')':
      return basicToken(TokenKind::SepCloseParen, input::Span{m_inputPos});
    case '{':
      return basicToken(TokenKind::SepOpenCurly, input::Span{m_inputPos});
    case '}':
      return basicToken(TokenKind::SepCloseCurly, input::Span{m_inputPos});
    case '[':
      if (peekChar(0) == ']') {
        consumeChar();
        return basicToken(TokenKind::OpSquareSquare, input::Span{m_inputPos - 1, m_inputPos});
      }
      return basicToken(TokenKind::SepOpenSquare, input::Span{m_inputPos});
    case ']':
      return basicToken(TokenKind::SepCloseSquare, input::Span{m_inputPos});
    case ',':
      return basicToken(TokenKind::SepComma, input::Span{m_inputPos});
    case ':':
      if (peekChar(0) == ':') {
        consumeChar();
        return basicToken(TokenKind::OpColonColon, input::Span{m_inputPos - 1, m_inputPos});
      }
      return basicToken(TokenKind::SepColon, input::Span{m_inputPos});
    case '#':
      return nextStaticInt();
    case ' ':
    case '\t':
    case '\n':
    case '\r':
      break; // Skip whitespace.
    case '0':
      switch (peekChar(0)) {
      case 'x':
      case 'X':
        consumeChar();
        return nextLitNumberHex();
      case 'b':
      case 'B':
        consumeChar();
        return nextLitNumberBinary();
      }
      [[fallthrough]];
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return nextLitNumber(c);
    case '"':
      return nextLitStr();
    case '\'':
      return nextLitChar();
    case '_': {
      const auto& nextChar = peekChar(0);
      if (isWordStart(nextChar) || isDigit(nextChar) || nextChar == '_') {
        return nextWordToken(c);
      }
      return basicToken(TokenKind::Discard, input::Span{m_inputPos});
    }
    default:
      if (isWordStart(c)) {
        return nextWordToken(c);
      }
      return errInvalidChar(c, input::Span(m_inputPos, m_inputPos));
    }
  }
}

auto LexerImpl::nextLitNumber(const char mostSignficantChar) -> Token {
  assert(isdigit(mostSignficantChar) || mostSignficantChar == '.');

  const uint64_t base = 10;
  const auto startPos = m_inputPos;
  auto passedDecPoint = mostSignficantChar == '.';
  uint64_t result     = passedDecPoint ? 0 : mostSignficantChar - '0';
  assert(result <= 9);

  auto tooBig              = false;
  auto containsInvalidChar = false;
  auto isLong              = false;
  uint64_t divider         = 1;
  char curChar             = mostSignficantChar;
  while (!isTokenSeperator(peekChar(0)) || peekChar(0) == '.') {
    curChar = consumeChar();
    if (isDigit(curChar)) {
      const auto add = curChar - '0';
      if (result > ((std::numeric_limits<uint64_t>::max() - add) / base)) {
        tooBig = true;
        continue;
      }
      result = result * base + add;
      if (passedDecPoint) {
        if (divider > (std::numeric_limits<uint64_t>::max() / base)) {
          tooBig = true;
          continue;
        }
        divider = divider * base;
      }
    } else if (curChar == '_') {
      continue; // Ignore underscores as legal digit seperators.
    } else if (!passedDecPoint && curChar == '.') {
      passedDecPoint = true;
    } else if (!passedDecPoint && (curChar == 'l' || curChar == 'L')) {
      isLong = true;
      break;
    } else {
      containsInvalidChar = true;
    }
  }

  const auto span = input::Span{startPos, m_inputPos};
  if (containsInvalidChar) {
    return errLitNumberInvalidChar(span);
  }
  if (curChar == '_') {
    return errLitNumberEndsWithSeperator(span);
  }
  if (curChar == '.') {
    return errLitNumberEndsWithDecimalPoint(span);
  }

  // Float.
  if (passedDecPoint) {
    if (tooBig) {
      // This is not fully correct as it might still be representable by a float but things get
      // allot more complicated if we cannot store the result and the divider as uint64's.
      return errLitFloatUnrepresentable(span);
    }
    /* For floats we divide the result by the divider. This still stuffers two rounding errors
    but its allot better then rounding in the loop. */
    return litFloatToken(static_cast<float>(result) / static_cast<float>(divider), span);
  }

  // Long.
  if (isLong) {
    if (tooBig || result > std::numeric_limits<int64_t>::max()) {
      return errLitLongTooBig(span);
    }
    return litLongToken(static_cast<int64_t>(result), span);
  }

  // Int.
  if (tooBig || result > std::numeric_limits<int32_t>::max()) {
    return errLitIntTooBig(span);
  }
  return litIntToken(static_cast<int32_t>(result), span);
}

auto LexerImpl::nextLitNumberHex() -> Token {
  const auto startPos = m_inputPos - 1; // Take the '0x' prefix into account.

  uint64_t result          = 0;
  char curChar             = 'x';
  auto containsInvalidChar = false;
  auto isLong              = false;
  auto tooBig              = false;
  while (!isTokenSeperator(peekChar(0))) {
    curChar        = consumeChar();
    uint64_t toAdd = 0;
    if (isDigit(curChar)) {
      toAdd = (curChar - '0');
    } else if (curChar >= 'a' && curChar <= 'f') {
      toAdd = (curChar - ('a' - 10)); // NOLINT: Magic numbers
    } else if (curChar >= 'A' && curChar <= 'F') {
      toAdd = (curChar - ('A' - 10)); // NOLINT: Magic numbers
    } else if (curChar == '_') {
      continue; // Ignore underscores as legal digit seperators.
    } else if (curChar == 'l' || curChar == 'L') {
      isLong = true;
      break;
    } else {
      containsInvalidChar = true;
    }

    // Check if the result would overflow.
    if (result > (std::numeric_limits<uint64_t>::max() - toAdd) >> 4U) {
      tooBig = true;
    } else {
      // Shift up the result by one 'nibble' and add the digit.
      result = (result << 4U) + toAdd;
    }
  }

  const auto span = input::Span{startPos, m_inputPos};
  if (containsInvalidChar) {
    return errLitHexInvalidChar(span);
  }
  if (curChar == '_') {
    return errLitNumberEndsWithSeperator(span);
  }

  // Long.
  if (isLong) {
    if (tooBig) {
      return errLitLongTooBig(span);
    }
    // Reinterpret because we support negative hex literals.
    return litLongToken(reinterpret_cast<int64_t&>(result), span); // NOLINT: Reinterpret cast
  }

  // Int.
  if (tooBig || result > std::numeric_limits<uint32_t>::max()) {
    return errLitIntTooBig(span);
  }
  // Reinterpret because we support negative hex literals.
  return litIntToken(reinterpret_cast<int32_t&>(result), span); // NOLINT: Reinterpret cast
}

auto LexerImpl::nextLitNumberBinary() -> Token {
  const auto startPos = m_inputPos - 1; // Take the '0b' prefix into account.

  uint64_t result          = 0;
  char curChar             = 'b';
  auto containsInvalidChar = false;
  auto isLong              = false;
  auto tooBig              = false;
  while (!isTokenSeperator(peekChar(0))) {
    curChar = consumeChar();
    if (curChar == '0' || curChar == '1') {
      if (result > ((std::numeric_limits<uint64_t>::max() - (curChar - '0')) >> 1U)) {
        tooBig = true;
      }
      result = (result << 1U) + (curChar - '0');
    } else if (curChar == '_') {
      continue; // Ignore underscores as legal digit seperators.
    } else if (curChar == 'l' || curChar == 'L') {
      isLong = true;
      break;
    } else {
      containsInvalidChar = true;
    }
  }

  const auto span = input::Span{startPos, m_inputPos};
  if (containsInvalidChar) {
    return errLitBinaryInvalidChar(span);
  }
  if (curChar == '_') {
    return errLitNumberEndsWithSeperator(span);
  }

  // Long.
  if (isLong) {
    if (tooBig) {
      return errLitLongTooBig(span);
    }
    // Reinterpret because we support negative binary literals.
    return litLongToken(reinterpret_cast<int64_t&>(result), span); // NOLINT: Reinterpret cast
  }

  // Int.
  if (tooBig || result > std::numeric_limits<uint32_t>::max()) {
    return errLitIntTooBig(span);
  }
  // Reinterpret because we support negative binary literals.
  return litIntToken(reinterpret_cast<int32_t&>(result), span); // NOLINT: Reinterpret cast
}

auto LexerImpl::nextLitStr() -> Token {
  // Starting '"' already consumed by caller.
  const auto startPos = m_inputPos;
  std::string result{};

  auto invalidEscapeSequence = false;
  while (true) {
    const auto c = consumeChar();
    switch (c) {
    case '\0':
    case '\r':
    case '\n':
      return erLitStrUnterminated(input::Span{startPos, m_inputPos});
    case '\\': {
      // Backslash is used to start an escape sequence.
      const auto unescapedC = input::unescape(consumeChar());
      if (unescapedC) {
        result += unescapedC.value();
      } else {
        invalidEscapeSequence = true;
      }
      break;
    }
    case '"': {
      const auto span = input::Span{startPos, m_inputPos};
      if (invalidEscapeSequence) {
        return errLitStrInvalidEscape(span);
      }
      return litStrToken(std::move(result), span);
    }
    default:
      result += c;
      break;
    }
  }
}

auto LexerImpl::nextLitChar() -> Token {
  // Starting quote already consumed by caller.
  const auto startPos = m_inputPos;

  auto tooBig                = false;
  auto invalidEscapeSequence = false;
  uint8_t c                  = consumeChar();
  if (c == '\'') {
    return erLitCharEmpty(input::Span{startPos, m_inputPos});
  }
  if (c == '\\') {
    // Backslash is used to start an escape sequence.
    const auto unescapedC = input::unescape(consumeChar());
    if (unescapedC) {
      c = unescapedC.value();
    } else {
      invalidEscapeSequence = true;
    }
  }
  while (true) {
    switch (consumeChar()) {
    case '\0':
    case '\r':
    case '\n':
      return errLitCharUnterminated(input::Span{startPos, m_inputPos});
    case '\'': {
      const auto span = input::Span{startPos, m_inputPos};
      if (tooBig) {
        return errLitCharTooBig(span);
      }
      if (invalidEscapeSequence) {
        return errLitCharInvalidEscape(span);
      }
      return litCharToken(c, span);
    }
    default:
      tooBig = true;
      break;
    }
  }
}

auto LexerImpl::nextStaticInt() -> Token {
  const auto startPos = m_inputPos;

  uint64_t val             = 0;
  bool atLeastOneDigit     = false;
  bool containsInvalidChar = false;
  bool tooBig              = false;
  while (!isTokenSeperator(peekChar(0))) {
    auto c = consumeChar();
    if (isDigit(c)) {
      val             = val * 10 + c - '0';
      atLeastOneDigit = true;
      if (val > std::numeric_limits<int32_t>::max()) {
        tooBig = true;
      }
    } else {
      containsInvalidChar = true;
    }
  }

  const auto span = input::Span{startPos, m_inputPos};
  if (containsInvalidChar) {
    return errStaticIntInvalidChar(span);
  }
  if (!atLeastOneDigit) {
    return errStaticIntNoDigits(span);
  }
  if (tooBig) {
    return errStaticIntTooBig(span);
  }
  return staticIntToken(static_cast<int32_t>(val), span);
}

auto LexerImpl::nextWordToken(const char startingChar) -> Token {
  const auto startPos = m_inputPos;
  std::string result(1, startingChar);

  auto invalidCharacter = false;
  auto illegalSequence  = startingChar == '_' && peekChar(0) == '_';
  while (!isTokenSeperator(peekChar(0))) {
    const auto c = consumeChar();
    if (isWordContinuation(c) || isDigit(c)) {
      result += c;
    } else if (c == '_') {
      if (peekChar(0) == '_') {
        illegalSequence = true;
      } else {
        result += c;
      }
    } else {
      invalidCharacter = true;
    }
  }
  const auto span = input::Span{startPos, m_inputPos};

  // Check if word is a literal.
  if (result == "true") {
    return litBoolToken(true, span);
  }
  if (result == "false") {
    return litBoolToken(false, span);
  }

  // Check if word is a keyword.
  const auto optKw = getKeyword(result);
  if (optKw) {
    return keywordToken(optKw.value(), span);
  }

  // If word is not a literal or a keyword its assumed to be an identifier.
  if (invalidCharacter) {
    return errIdentifierIllegalCharacter(span);
  }
  if (illegalSequence) {
    return errIdentifierIllegalSequence(span);
  }
  return identiferToken(std::move(result), span);
}

auto LexerImpl::nextLineComment() -> Token {
  // First '/' already consumed by caller.
  const auto startPos = m_inputPos;
  assert(peekChar(0) == '/');
  consumeChar(); // Consume second '/'.

  auto comment = std::string{};
  while (true) {
    const auto c = consumeChar();
    switch (c) {
    case '\r':
      break;
    case '\0':
    case '\n':
      return lineCommentToken(comment, input::Span{startPos, m_inputPos});
    default:
      comment += c;
      break;
    }
  }
}

auto LexerImpl::consumeChar() -> char {
  char val;
  if (!m_readBuffer.empty()) {
    val = m_readBuffer.front();
    m_readBuffer.pop_front();
  } else {
    val = getFromInput();
  }

  if (val != '\0') {
    m_inputPos++;
  }
  return val;
}

auto LexerImpl::peekChar(const size_t ahead) -> char& {
  for (auto i = m_readBuffer.size(); i <= ahead; i++) {
    m_readBuffer.push_back(getFromInput());
  }

  return m_readBuffer[ahead];
}

} // namespace internal

// Explicit instantiations.
template class Lexer<char*, char*>;
template class Lexer<std::string::iterator, std::string::iterator>;
template class Lexer<std::string::const_iterator, std::string::const_iterator>;
template class Lexer<std::istream_iterator<char>, std::istream_iterator<char>>;
template class Lexer<std::istreambuf_iterator<char>, std::istreambuf_iterator<char>>;

} // namespace lex
