#pragma once
#include "lex/token_type.hpp"
#include <iostream>

namespace lex {

enum class TokenCat { Operator, Seperator, Literal, Keyword, Identifier, Error, Unknown };

auto lookupCat(TokenType type) -> TokenCat;

auto operator<<(std::ostream& out, const TokenCat& rhs) -> std::ostream&;

} // namespace lex
