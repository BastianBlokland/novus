#pragma once
#include "input/span.hpp"
#include "lex/keyword.hpp"
#include "lex/token_cat.hpp"
#include "lex/token_kind.hpp"
#include "lex/token_payload.hpp"
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

namespace lex {

class Token final {
public:
  Token();
  Token(TokenKind kind, std::unique_ptr<TokenPayload> payload, input::Span span);
  Token(const Token& rhs);
  Token(Token&& rhs) noexcept;

  auto operator=(const Token& rhs) -> Token&;
  auto operator=(Token&& rhs) noexcept -> Token&;

  auto operator==(const Token& rhs) const noexcept -> bool;
  auto operator!=(const Token& rhs) const noexcept -> bool;

  ~Token() = default;

  [[nodiscard]] auto getKind() const noexcept { return m_kind; }

  [[nodiscard]] auto getSpan() const noexcept { return m_span; }

  [[nodiscard]] auto getPayload() const noexcept { return m_payload.get(); }

  template <typename PayloadT>
  [[nodiscard]] auto getPayload() const -> const PayloadT* {
    return dynamic_cast<PayloadT*>(m_payload.get());
  }

  [[nodiscard]] auto isEnd() const noexcept { return m_kind == TokenKind::End; }

  [[nodiscard]] auto isError() const noexcept { return m_kind == TokenKind::Error; }

  [[nodiscard]] auto getCat() const -> TokenCat { return lookupCat(m_kind); }

  [[nodiscard]] auto str() const -> std::string;

private:
  TokenKind m_kind;
  std::unique_ptr<TokenPayload> m_payload;
  input::Span m_span;
};

auto operator<<(std::ostream& out, const Token& rhs) -> std::ostream&;

// Factories.
auto endToken(input::Span span = input::Span{0}) -> Token;

auto basicToken(TokenKind kind, input::Span span = input::Span{0}) -> Token;

auto errorToken(std::string msg, input::Span span = input::Span{0}) -> Token;

auto litIntToken(int32_t val, input::Span span = input::Span{0}) -> Token;

auto litLongToken(int64_t val, input::Span span = input::Span{0}) -> Token;

auto litFloatToken(float val, input::Span span = input::Span{0}) -> Token;

auto litBoolToken(bool val, input::Span span = input::Span{0}) -> Token;

auto litStrToken(std::string val, input::Span span = input::Span{0}) -> Token;

auto litCharToken(uint8_t val, input::Span span = input::Span{0}) -> Token;

auto staticIntToken(int32_t val, input::Span span = input::Span{0}) -> Token;

auto keywordToken(Keyword keyword, input::Span span = input::Span{0}) -> Token;

auto identiferToken(std::string id, input::Span span = input::Span{0}) -> Token;

auto lineCommentToken(std::string comment, input::Span span = input::Span{0}) -> Token;

} // namespace lex
