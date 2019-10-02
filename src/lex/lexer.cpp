#include "lex/lexer.hpp"
#include <cctype>
#include <iostream>
#include <limits>
#include <map>
#include <string>

namespace lex {

template <typename InputItr> auto Lexer<InputItr>::next() -> Token {
  while (true) {
    const auto c = consumeChar();
    switch (c) {
    case '\0':
      return Token::endToken(SourceSpan{_inputPos >= 0 ? _inputPos : 0});
    case '+':
      return Token::basicToken(TokenType::OpPlus, SourceSpan{_inputPos});
    case '-':
      return Token::basicToken(TokenType::OpMinus, SourceSpan{_inputPos});
    case '*':
      return Token::basicToken(TokenType::OpStar, SourceSpan{_inputPos});
    case '/':
      return Token::basicToken(TokenType::OpSlash, SourceSpan{_inputPos});
    case '?':
      return Token::basicToken(TokenType::OpQMark, SourceSpan{_inputPos});
    case ':':
      return Token::basicToken(TokenType::OpColon, SourceSpan{_inputPos});
    case '(':
      return Token::basicToken(TokenType::SepOpenParan, SourceSpan{_inputPos});
    case ')':
      return Token::basicToken(TokenType::SepCloseParan, SourceSpan{_inputPos});
    case ',':
      return Token::basicToken(TokenType::SepComma, SourceSpan{_inputPos});
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
      return Token::errorToken(SourceSpan(_inputPos, _inputPos), "Invalid character");
    }
  }
}

template <typename InputItr> auto Lexer<InputItr>::nextLitInt(char mostSignficantChar) -> Token {
  assert(std::isdigit(mostSignficantChar));

  const auto startPos = _inputPos;
  uint32_t result = mostSignficantChar - '0';
  assert(result >= 0 && result <= 9);

  bool tooBig = false;
  while (std::isdigit(peekChar(0))) {
    const uint64_t base = 10;
    uint64_t newResult = result * base + (consumeChar() - '0');
    if (newResult > std::numeric_limits<uint32_t>::max()) {
      tooBig = true;
    }

    result = newResult;
  }

  const auto span = SourceSpan{startPos, _inputPos};
  return tooBig ? Token::errorToken(span, "Number literal too big")
                : Token::litIntToken(span, result);
}

template <typename InputItr> auto Lexer<InputItr>::nextLitStr() -> Token {
  // Starting '"' already consumed by caller.
  const auto startPos = _inputPos;
  std::string result{};
  while (true) {
    auto c = consumeChar();
    switch (c) {
    case '\0':
    case '\r':
    case '\n':
      return Token::errorToken(SourceSpan{startPos, _inputPos}, "Unterminated string literal");
    case '"':
      // Allow escaping of double quotes by using an additional double quote.
      if (peekChar(0) == '"') {
        result += consumeChar();
        break;
      }
      return Token::litStrToken(SourceSpan{startPos, _inputPos}, result);
    default:
      result += c;
      break;
    }
  }
}

template <typename InputItr> auto Lexer<InputItr>::nextWordToken(char startingChar) -> Token {
  const auto startPos = _inputPos;
  std::string result(1, startingChar);
  while (std::isalpha(peekChar(0))) {
    result += consumeChar();
  }

  const auto span = SourceSpan{startPos, _inputPos};

  // Check if word is a literal.
  if (result == "true") {
    return Token::litBoolToken(span, true);
  }
  if (result == "false") {
    return Token::litBoolToken(span, false);
  }

  // Check if word is a keyword.
  static const std::map<std::string, Keyword> keywordTable = {{"fun", Keyword::Function}};
  const auto keywordSearch = keywordTable.find(result);
  if (keywordSearch != keywordTable.end()) {
    const auto keyword = keywordSearch->second;
    return Token::keywordToken(span, keyword);
  }

  // It word is not a literal or a keyword its assumed to be an identifier.
  return Token::identiferToken(span, result);
}

template <typename InputItr> auto Lexer<InputItr>::consumeChar() -> char {
  char val;
  if (!_readBuffer.empty()) {
    val = _readBuffer.front();
    _readBuffer.pop_front();
  } else {
    val = getFromInput();
  }

  if (val != '\0') {
    _inputPos++;
  }
  return val;
}

template <typename InputItr> auto Lexer<InputItr>::peekChar(const int ahead) -> char {
  for (auto i = _readBuffer.size(); i <= ahead; i++) {
    _readBuffer.push_back(getFromInput());
  }

  return _readBuffer[ahead];
}

template <typename InputItr> auto Lexer<InputItr>::getFromInput() -> char {
  if (_input == _inputEnd) {
    return '\0';
  }
  const auto val = *_input;
  _input++;
  return val;
}

// Explicit instantiations.
template class Lexer<char*>;
template class Lexer<std::string::iterator>;
template class Lexer<std::istream_iterator<char>>;
template class Lexer<std::istreambuf_iterator<char>>;

} // namespace lex
