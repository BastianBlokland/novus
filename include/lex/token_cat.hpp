#pragma once
#include "lex/token_type.hpp"
#include <iostream>

namespace lex {

enum class TokenCat { Operator, Seperator, Literal, Keyword, Identifier, Error, Unknown };

auto lookupCat(const TokenType type) -> TokenCat;

std::ostream& operator<<(std::ostream& out, const TokenCat& rhs);

} // namespace lex
