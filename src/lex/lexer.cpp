#include "lex/lexer.hpp"
#include "lex/error.hpp"
#include <cassert>
#include <cctype>
#include <iostream>
#include <limits>
#include <optional>
#include <string>

namespace lex {

template <typename InputItr> auto Lexer<InputItr>::next() -> Token {
  while (true) {
    const auto c = consumeChar();
    switch (c) {
    case '\0':
      return endToken(SourceSpan{m_inputPos >= 0 ? m_inputPos : 0});
    case '+':
      return basicToken(TokenType::OpPlus, SourceSpan{m_inputPos});
    case '-':
      return basicToken(TokenType::OpMinus, SourceSpan{m_inputPos});
    case '*':
      return basicToken(TokenType::OpStar, SourceSpan{m_inputPos});
    case '/':
      return basicToken(TokenType::OpSlash, SourceSpan{m_inputPos});
    case '?':
      return basicToken(TokenType::OpQMark, SourceSpan{m_inputPos});
    case ':':
      return basicToken(TokenType::OpColon, SourceSpan{m_inputPos});
    case '(':
      return basicToken(TokenType::SepOpenParan, SourceSpan{m_inputPos});
    case ')':
      return basicToken(TokenType::SepCloseParan, SourceSpan{m_inputPos});
    case ',':
      return basicToken(TokenType::SepComma, SourceSpan{m_inputPos});
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
    default:
      if (std::isalnum(c)) {
        return nextWordToken(c);
      }
      return errInvalidCharacter(c, SourceSpan(m_inputPos, m_inputPos));
    }
  }
}

template <typename InputItr> auto Lexer<InputItr>::nextLitInt(char mostSignficantChar) -> Token {
  const static char seperator = '_'; // Legal seperator between digits.

  assert(std::isdigit(mostSignficantChar));

  const auto startPos = m_inputPos;
  int32_t result = mostSignficantChar - '0';
  assert(result >= 0 && result <= 9);

  auto tooBig = false;
  auto invalidCharacter = false;
  auto isSeperator = false;
  while (true) {
    const auto nextC = peekChar(0);
    const auto isDigit = std::isdigit(nextC);
    if (!isDigit && nextC != seperator && !std::isalnum(nextC)) {
      break;
    }
    isSeperator = nextC == seperator;
    const auto c = consumeChar();
    if (isDigit) {
      const uint64_t base = 10;
      const uint64_t newResult = result * base + (c - '0');
      if (newResult > std::numeric_limits<int32_t>::max()) {
        tooBig = true;
      } else {
        result = newResult;
      }
    } else if (isSeperator) {
      continue; // Ignore seperator characters.
    } else {
      invalidCharacter = true;
    }
  }

  const auto span = SourceSpan{startPos, m_inputPos};
  if (invalidCharacter) {
    return errLitIntInvalidChar(span);
  }
  if (isSeperator) {
    return errLitIntEndsWithSeperator(span);
  }
  if (tooBig) {
    return errLitIntTooBig(span);
  }
  return litIntToken(result, span);
}

template <typename InputItr> auto Lexer<InputItr>::nextLitStr() -> Token {
  // Starting '"' already consumed by caller.
  const auto startPos = m_inputPos;
  std::string result{};
  while (true) {
    auto c = consumeChar();
    switch (c) {
    case '\0':
    case '\r':
    case '\n':
      return errUnterminatedStringLiteral(SourceSpan{startPos, m_inputPos});
    case '"':
      // Allow escaping of double quotes by using an additional double quote.
      if (peekChar(0) == '"') {
        result += consumeChar();
        break;
      }
      return litStrToken(result, SourceSpan{startPos, m_inputPos});
    default:
      result += c;
      break;
    }
  }
}

template <typename InputItr> auto Lexer<InputItr>::nextWordToken(char startingChar) -> Token {
  const auto startPos = m_inputPos;
  std::string result(1, startingChar);
  while (std::isalpha(peekChar(0))) {
    result += consumeChar();
  }

  const auto span = SourceSpan{startPos, m_inputPos};

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
  return identiferToken(result, span);
}

template <typename InputItr> auto Lexer<InputItr>::consumeChar() -> char {
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

template <typename InputItr> auto Lexer<InputItr>::peekChar(const int ahead) -> char {
  for (auto i = m_readBuffer.size(); i <= ahead; i++) {
    m_readBuffer.push_back(getFromInput());
  }

  return m_readBuffer[ahead];
}

template <typename InputItr> auto Lexer<InputItr>::getFromInput() -> char {
  if (m_input == m_inputEnd) {
    return '\0';
  }
  const auto val = *m_input;
  m_input++;
  return val;
}

// Explicit instantiations.
template class Lexer<char*>;
template class Lexer<std::string::iterator>;
template class Lexer<std::istream_iterator<char>>;
template class Lexer<std::istreambuf_iterator<char>>;

} // namespace lex
