#pragma once
#include "lex/token.hpp"
#include "lex/token_itr.hpp"
#include <deque>
#include <iterator>
#include <utility>

using std::is_same;
using std::iterator_traits;

namespace lex {

template <typename InputItr> class Lexer final {

  static_assert(is_same<typename iterator_traits<InputItr>::value_type, char>::value,
                "Valuetype of input iterator has to be 'char'");

public:
  Lexer() = delete;
  Lexer(InputItr inputBegin, const InputItr inputEnd)
      : m_input{inputBegin}, m_inputEnd{inputEnd}, m_inputPos{-1}, m_readBuffer{} {}

  auto begin() -> TokenItr<Lexer> { return TokenItr{*this}; }

  auto end() -> TokenItr<NopTokenSource> { return TokenItr{}; }

  auto next() -> Token;

private:
  InputItr m_input;
  const InputItr m_inputEnd;
  int m_inputPos;
  std::deque<char> m_readBuffer;

  auto nextLitInt(const char mostSignficantChar) -> Token;
  auto nextLitStr() -> Token;
  auto nextWordToken(const char startingChar) -> Token;

  auto consumeChar() -> char;
  auto peekChar(const int ahead) -> char;
  auto getFromInput() -> char;
};

} // namespace lex
