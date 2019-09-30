#include "lex/lexer.hpp"
#include <iterator>

namespace lex {

template <typename InputItr>
Lexer<InputItr>::Lexer(InputItr inputStart, InputItr inputEnd)
    : _inputStart{inputStart}, _inputEnd{inputEnd} {}

template <typename InputItr> auto Lexer<InputItr>::next() -> Token {
  return Token{TokenType::Eof, SourceSpan{0, 1}};
}

// Explicit instantiations:
template class Lexer<char *>;
template class Lexer<std::istream_iterator<char>>;

} // namespace lex
