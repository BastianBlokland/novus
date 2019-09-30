#pragma once
#include "lex/sourcespan.hpp"
#include "lex/tokentype.hpp"

namespace lex {

class Token final {
public:
  Token() = delete;
  Token(const TokenType type, const SourceSpan span);

  auto getType() const -> TokenType;
  auto getSpan() const -> SourceSpan;

private:
  const TokenType _type;
  const SourceSpan _span;
};

} // namespace lex
