#pragma once
#include "lex/token.hpp"
#include <iterator>
#include <utility>

namespace lex {

class TokenItrTraits {
public:
  using difference_type   = ptrdiff_t;
  using value_type        = Token;
  using pointer           = Token*;
  using reference         = Token&&;
  using iterator_category = std::input_iterator_tag;
};

// Iterator that wraps a source that provides a token when 'next()' is called.
template <typename TokenSource>
class TokenItr final : public TokenItrTraits {

  static_assert(
      std::is_same<decltype(std::declval<TokenSource&>().next()), Token>::value,
      "TokenSource has to have a 'next' function returning a token.");

public:
  TokenItr() : m_source{nullptr}, m_current{} {}

  explicit TokenItr(TokenSource& tokenSource) : m_source{&tokenSource} {
    // Set the initial value.
    m_current = tokenSource.next();
  }

  auto operator*() -> Token&& { return std::move(m_current); }

  auto operator-> () -> Token* { return &m_current; }

  auto operator==(const TokenItr& rhs) noexcept -> bool { return m_current == rhs.m_current; }

  auto operator!=(const TokenItr& rhs) noexcept -> bool { return m_current != rhs.m_current; }

  auto operator++() -> void { m_current = getToken(); }

private:
  TokenSource* m_source;
  Token m_current;

  auto getToken() -> Token {
    if (m_source) {
      return m_source->next();
    }
    return endToken();
  }
};

} // namespace lex
