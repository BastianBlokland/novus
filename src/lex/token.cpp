#include "lex/token.hpp"

namespace lex {

Token::Token(const TokenType type, const SourceSpan span)
    : _type{type}, _span{span} {}

auto Token::getType() const -> TokenType { return _type; }
auto Token::getSpan() const -> SourceSpan { return _span; }

} // namespace lex
