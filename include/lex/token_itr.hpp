#pragma once
#include "lex/token.hpp"
#include <utility>

namespace lex {

class TokenItrTraits {
public:
  using difference_type   = ptrdiff_t;
  using value_type        = Token;
  using pointer           = const Token*;
  using reference         = const Token&;
  using iterator_category = std::input_iterator_tag;
};

template <typename TokenSource>
class TokenItr final : public TokenItrTraits {

  static_assert(
      std::is_same<decltype(std::declval<TokenSource&>().next()), Token>::value,
      "TokenSource has to have a 'next' function returning a token.");

public:
  TokenItr() : m_source{nullptr} {}

  explicit TokenItr(TokenSource& tokenSource) : m_source{&tokenSource} {
    // Set the initial value.
    m_current = tokenSource.next();
  }

  auto operator*() -> const Token& { return m_current; }

  auto operator-> () -> const Token* { return &m_current; }

  auto operator==(const TokenItr& rhs) noexcept -> bool { return m_current == rhs.m_current; }

  auto operator!=(const TokenItr& rhs) noexcept -> bool { return m_current != rhs.m_current; }

  auto operator++() { m_current = getToken(); }

private:
  TokenSource* m_source;
  Token m_current;

  auto getToken() -> Token {
    if (m_source) {
      return m_source->next();
    }
    return endToken(SourceSpan{0});
  }
};

} // namespace lex
