#pragma once
#include "lex/token.hpp"
#include <utility>

namespace lex {

class TokenItrBase {
public:
  using difference_type   = ptrdiff_t;
  using value_type        = Token;
  using pointer           = const Token*;
  using reference         = const Token&;
  using iterator_category = std::input_iterator_tag;

  TokenItrBase() : m_current{} {}

  auto operator*() -> const Token& { return m_current; }

  auto operator-> () -> const Token* { return &m_current; }

  auto operator==(const TokenItrBase& rhs) noexcept -> bool { return m_current == rhs.m_current; }

  auto operator!=(const TokenItrBase& rhs) noexcept -> bool { return m_current != rhs.m_current; }

  auto operator++() { m_current = getToken(); }

protected:
  auto setCurrent(Token token) { m_current = std::move(token); }

private:
  Token m_current;

  virtual auto getToken() -> Token = 0;
};

class NopTokenSource final {
public:
  static auto next() -> Token { return endToken(SourceSpan{0}); }
};

template <typename TokenSource = NopTokenSource>
class TokenItr final : public TokenItrBase {

  static_assert(
      std::is_same<decltype(std::declval<TokenSource&>().next()), Token>::value,
      "TokenSource has to have a 'next' function returning a token.");

public:
  TokenItr() : m_source{nullptr} {}

  explicit TokenItr(TokenSource& tokenSource) : m_source{&tokenSource} {
    // Set the initial value.
    setCurrent(tokenSource.next());
  }

private:
  TokenSource* m_source;

  auto getToken() -> Token override {
    if (m_source) {
      return m_source->next();
    }
    return endToken();
  }
};

} // namespace lex
