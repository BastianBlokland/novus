#include "lex/lexer.hpp"
#include "char_escape.hpp"
#include "lex/error.hpp"

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
      return basicToken(TokenKind::OpPlus, input::Span{m_inputPos});
    case '-':
      if (peekChar(0) == '>') {
        consumeChar();
        return basicToken(TokenKind::SepArrow, input::Span{m_inputPos - 1, m_inputPos});
      }
      return basicToken(TokenKind::OpMinus, input::Span{m_inputPos});
    case '*':
      return basicToken(TokenKind::OpStar, input::Span{m_inputPos});
    case '/':
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
    case '=':
      if (peekChar(0) == '=') {
        consumeChar();
        return basicToken(TokenKind::OpEqEq, input::Span{m_inputPos - 1, m_inputPos});
      }
      return basicToken(TokenKind::OpEq, input::Span{m_inputPos});
    case '!':
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
      return basicToken(TokenKind::OpLe, input::Span{m_inputPos});
    case '>':
      if (peekChar(0) == '=') {
        consumeChar();
        return basicToken(TokenKind::OpGtEq, input::Span{m_inputPos - 1, m_inputPos});
      }
      return basicToken(TokenKind::OpGt, input::Span{m_inputPos});
    case ';':
      return basicToken(TokenKind::OpSemi, input::Span{m_inputPos});
    case '?':
      return basicToken(TokenKind::OpQMark, input::Span{m_inputPos});
    case '(':
      return basicToken(TokenKind::SepOpenParen, input::Span{m_inputPos});
    case ')':
      return basicToken(TokenKind::SepCloseParen, input::Span{m_inputPos});
    case ',':
      return basicToken(TokenKind::SepComma, input::Span{m_inputPos});
    case ':':
      return basicToken(TokenKind::SepColon, input::Span{m_inputPos});
    case ' ':
    case '\t':
    case '\n':
    case '\r':
      break; // Skip whitespace.
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return nextLitInt(c);
    case '"':
      return nextLitStr();
    case '_': {
      const auto& nextChar = peekChar(0);
      if (isWordStart(nextChar) || isDigit(nextChar) || nextChar == '_') {
        return nextWordToken(c);
      }
      // Current '_' is not used as a valid token own.
      return errInvalidChar(c, input::Span(m_inputPos, m_inputPos));
    }
    default:
      if (isWordStart(c)) {
        return nextWordToken(c);
      }
      return errInvalidChar(c, input::Span(m_inputPos, m_inputPos));
    }
  }
}

auto LexerImpl::nextLitInt(const char mostSignficantChar) -> Token {
  assert(isdigit(mostSignficantChar));

  const auto startPos = m_inputPos;
  int32_t result      = mostSignficantChar - '0';
  assert(result >= 0 && result <= 9);

  auto tooBig              = false;
  auto containsInvalidChar = false;
  char curChar             = mostSignficantChar;
  while (!isTokenSeperator(peekChar(0))) {
    curChar = consumeChar();
    if (isDigit(curChar)) {
      const uint64_t base      = 10;
      const uint64_t newResult = result * base + (curChar - '0');
      if (newResult > std::numeric_limits<int32_t>::max()) {
        tooBig = true;
      } else {
        result = newResult;
      }
    } else if (curChar == '_') {
      continue; // Ignore underscores as legal digit seperators.
    } else {
      containsInvalidChar = true;
    }
  }

  const auto span = input::Span{startPos, m_inputPos};
  if (containsInvalidChar) {
    return errLitIntInvalidChar(span);
  }
  if (curChar == '_') {
    return errLitIntEndsWithSeperator(span);
  }
  if (tooBig) {
    return errLitIntTooBig(span);
  }
  return litIntToken(result, span);
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
      const auto unescapedC = unescape(consumeChar());
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

  // It word is not a literal or a keyword its assumed to be an identifier.
  if (invalidCharacter) {
    return errIdentifierIllegalCharacter(span);
  }
  if (illegalSequence) {
    return errIdentifierIllegalSequence(span);
  }
  return identiferToken(std::move(result), span);
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

} // namespace lex
