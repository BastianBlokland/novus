#pragma once
#include "lex/token.hpp"
#include "lex/token_itr.hpp"
#include <deque>
#include <iterator>
#include <utility>
#include <vector>

namespace lex {

template <typename InputItr>
class Lexer final {

  static_assert(
      std::is_same<typename std::iterator_traits<InputItr>::value_type, char>::value,
      "Valuetype of input iterator has to be 'char'");

public:
  Lexer() = delete;
  Lexer(InputItr inputBegin, const InputItr inputEnd) :
      m_input{inputBegin}, m_inputEnd{inputEnd}, m_inputPos{-1}, m_readBuffer{} {}

  auto begin() -> TokenItr<Lexer> { return TokenItr{*this}; }

  auto end() -> TokenItr<Lexer> { return TokenItr<Lexer>{}; }

  auto next() -> Token;

private:
  InputItr m_input;
  const InputItr m_inputEnd;
  int m_inputPos;
  std::deque<char> m_readBuffer;

  auto nextLitInt(char mostSignficantChar) -> Token;
  auto nextLitStr() -> Token;
  auto nextWordToken(char startingChar) -> Token;

  auto consumeChar() -> char;
  auto peekChar(size_t ahead) -> char&;
  auto getFromInput() -> char;
};

// Utilities.
template <typename InputItr>
auto lexAll(InputItr inputBegin, const InputItr inputEnd) {
  auto lexer = Lexer{inputBegin, inputEnd};
  return std::vector<Token>{lexer.begin(), lexer.end()};
}

} // namespace lex
