#pragma once
#include "lex/keyword.hpp"
#include "lex/source_span.hpp"
#include "lex/token_cat.hpp"
#include "lex/token_payload.hpp"
#include "lex/token_type.hpp"
#include <iostream>
#include <string>
#include <utility>

namespace lex {

class Token final {
public:
  Token();
  Token(TokenType type, TokenPayload* payload, SourceSpan span);
  Token(const Token& rhs);
  Token(Token&& rhs) noexcept;

  auto operator=(const Token& rhs) -> Token&;
  auto operator=(Token&& rhs) noexcept -> Token&;

  auto operator==(const Token& rhs) const noexcept -> bool;
  auto operator!=(const Token& rhs) const noexcept -> bool;

  ~Token();

  [[nodiscard]] auto getType() const noexcept { return m_type; }

  [[nodiscard]] auto getSpan() const noexcept { return m_span; }

  [[nodiscard]] auto getPayload() const noexcept { return m_payload; }

  [[nodiscard]] auto isEnd() const noexcept { return m_type == TokenType::End; }

  [[nodiscard]] auto getCat() const -> TokenCat { return lookupCat(m_type); }

private:
  TokenType m_type;
  TokenPayload* m_payload;
  SourceSpan m_span;
};

auto operator<<(std::ostream& out, const Token& rhs) -> std::ostream&;

// Factories.
auto endToken(SourceSpan span = SourceSpan{0}) -> Token;

auto basicToken(TokenType type, SourceSpan span = SourceSpan{0}) -> Token;

auto errorToken(const std::string& msg, SourceSpan span = SourceSpan{0}) -> Token;

auto litIntToken(int32_t val, SourceSpan span = SourceSpan{0}) -> Token;

auto litBoolToken(bool val, SourceSpan span = SourceSpan{0}) -> Token;

auto litStrToken(const std::string& val, SourceSpan span = SourceSpan{0}) -> Token;

auto keywordToken(Keyword keyword, SourceSpan span = SourceSpan{0}) -> Token;

auto identiferToken(const std::string& id, SourceSpan span = SourceSpan{0}) -> Token;

} // namespace lex
