#pragma once
#include "lex/token_kind.hpp"
#include <iostream>

namespace lex {

enum class TokenCat { Operator, Seperator, Literal, Keyword, Identifier, Error, Unknown };

auto lookupCat(TokenKind kind) -> TokenCat;

auto operator<<(std::ostream& out, const TokenCat& rhs) -> std::ostream&;

} // namespace lex
