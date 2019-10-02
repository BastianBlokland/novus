#pragma once
#include "lex/token.hpp"
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
  Lexer(InputItr inputStart, const InputItr inputEnd)
      : _input{inputStart}, _inputEnd{inputEnd}, _inputPos{-1}, _readBuffer{} {}

  auto next() -> Token;

private:
  InputItr _input;
  const InputItr _inputEnd;
  int _inputPos;
  std::deque<char> _readBuffer;

  auto nextLitInt(const char mostSignficantChar) -> Token;
  auto nextLitStr() -> Token;
  auto nextWordToken(const char startingCharr) -> Token;

  auto consumeChar() -> char;
  auto peekChar(const int ahead) -> char;
  auto getFromInput() -> char;
};

} // namespace lex
