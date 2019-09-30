#pragma once
#include "lex/token.hpp"
#include <iterator>

using std::is_same;
using std::iterator_traits;

namespace lex {

template <typename InputItr> class Lexer final {
  static_assert(
      is_same<typename iterator_traits<InputItr>::value_type, char>::value,
      "Type of input iterator has to be char");

public:
  Lexer(InputItr inputStart, InputItr inputEnd);

private:
  InputItr _inputStart, _inputEnd;

  auto next() -> Token;
};

} // namespace lex
