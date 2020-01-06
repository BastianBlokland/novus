#pragma once
#include "lex/token.hpp"
#include "lex/token_itr.hpp"
#include <deque>
#include <vector>

namespace lex {

namespace internal {

class LexerImpl {
protected:
  LexerImpl() : m_inputPos{-1}, m_readBuffer{} {}

  auto next() -> Token;

private:
  int m_inputPos;
  std::deque<char> m_readBuffer;

  auto nextLitNumber(char mostSignficantChar) -> Token;
  auto nextLitIntHex() -> Token;
  auto nextLitIntBinary() -> Token;
  auto nextLitStr() -> Token;
  auto nextWordToken(char startingChar) -> Token;
  auto nextLineComment() -> Token;

  auto consumeChar() -> char;
  auto peekChar(size_t ahead) -> char&;

  virtual auto getFromInput() -> char = 0;
};

} // namespace internal

template <typename InputItrBegin, typename InputItrEnd>
class Lexer final : private internal::LexerImpl {

  static_assert(
      std::is_same<typename std::iterator_traits<InputItrBegin>::value_type, char>::value,
      "Valuetype of input iterator has to be 'char'");

public:
  Lexer() = delete;
  Lexer(InputItrBegin inputBegin, const InputItrEnd inputEnd) :
      m_input{inputBegin}, m_inputEnd{inputEnd} {}

  [[nodiscard]] auto next() -> Token { return LexerImpl::next(); }

  [[nodiscard]] auto begin() -> TokenItr<Lexer> { return TokenItr{*this}; }

  [[nodiscard]] auto end() -> TokenItr<Lexer> { return TokenItr<Lexer>{}; }

private:
  InputItrBegin m_input;
  const InputItrEnd m_inputEnd;

  auto getFromInput() -> char override {
    if (m_input == m_inputEnd) {
      return '\0';
    }
    auto val = *m_input;
    ++m_input;
    return std::move(val);
  }
};

// Utilities.
template <typename InputItrBegin, typename InputItrEnd>
auto lexAll(InputItrBegin inputBegin, const InputItrEnd inputEnd) {
  auto lexer = Lexer{inputBegin, inputEnd};
  return std::vector<Token>{lexer.begin(), lexer.end()};
}

} // namespace lex
