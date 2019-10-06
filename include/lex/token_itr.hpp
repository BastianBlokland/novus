#pragma once
#include "lex/token.hpp"
#include <utility>

using std::declval;
using std::is_same;

namespace lex {

class TokenItrBase {
public:
  using difference_type = ptrdiff_t;
  using value_type = Token;
  using pointer = const Token*;
  using reference = const Token&;
  using iterator_category = std::input_iterator_tag;

  TokenItrBase() : m_current{} {}

  auto operator*() -> const Token& { return m_current; }

  auto operator-> () -> const Token* { return &m_current; }

  auto operator==(const TokenItrBase& rhs) noexcept -> bool { return m_current == rhs.m_current; }

  auto operator!=(const TokenItrBase& rhs) noexcept -> bool { return m_current != rhs.m_current; }

protected:
  Token m_current;
};

class NopTokenSource final {
public:
  auto next() -> Token { return endToken(SourceSpan{0}); }
};

template <typename TokenSource = NopTokenSource> class TokenItr final : public TokenItrBase {

  static_assert(
      is_same<decltype(declval<TokenSource&>().next()), Token>::value,
      "TokenSource has to have a 'next' function returning a token.");

public:
  TokenItr() : m_source{nullptr} {}

  TokenItr(TokenSource& tokenSource) : m_source{&tokenSource} {
    // Set the initial value.
    m_current = tokenSource.next();
  }

  auto operator++() { advance(); }

private:
  TokenSource* m_source;

  auto advance() {
    if (m_source != nullptr)
      m_current = m_source->next();
  }
};

} // namespace lex
