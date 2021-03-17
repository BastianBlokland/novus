#pragma once
#include "lex/token_kind.hpp"
#include <iostream>

namespace lex {

// Token category.
enum class TokenCat {
  Operator,
  Seperator,
  Literal,
  Keyword,
  Identifier,
  Comment,
  Error,
  Unknown,
};

auto lookupCat(TokenKind kind) -> TokenCat;

auto operator<<(std::ostream& out, const TokenCat& rhs) -> std::ostream&;

} // namespace lex
