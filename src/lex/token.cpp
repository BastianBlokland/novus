#include "lex/token.hpp"
#include "lex/token_payload.hpp"
#include "lex/token_payload_comment.hpp"
#include "lex/token_payload_error.hpp"
#include "lex/token_payload_id.hpp"
#include "lex/token_payload_keyword.hpp"
#include "lex/token_payload_lit_bool.hpp"
#include "lex/token_payload_lit_char.hpp"
#include "lex/token_payload_lit_float.hpp"
#include "lex/token_payload_lit_int.hpp"
#include "lex/token_payload_lit_long.hpp"
#include "lex/token_payload_lit_string.hpp"
#include <memory>
#include <sstream>
#include <utility>

namespace lex {

Token::Token() : m_kind{TokenKind::End}, m_payload{nullptr}, m_span{0} {}

Token::Token(const TokenKind kind, std::unique_ptr<TokenPayload> payload, const input::Span span) :
    m_kind{kind}, m_payload{std::move(payload)}, m_span{span} {}

Token::Token(const Token& rhs) :
    m_kind{rhs.m_kind},
    m_payload{rhs.m_payload ? rhs.m_payload->clone() : nullptr},
    m_span{rhs.m_span} {}

Token::Token(Token&& rhs) noexcept :
    m_kind{rhs.m_kind}, m_payload{std::move(rhs.m_payload)}, m_span{rhs.m_span} {}

auto Token::operator=(const Token& rhs) -> Token& {
  if (this == &rhs) {
    return *this;
  }
  m_kind    = rhs.m_kind;
  m_span    = rhs.m_span;
  m_payload = rhs.m_payload ? rhs.m_payload->clone() : nullptr;
  return *this;
}

auto Token::operator=(Token&& rhs) noexcept -> Token& {
  if (this == &rhs) {
    return *this;
  }
  m_kind    = rhs.m_kind;
  m_span    = rhs.m_span;
  m_payload = std::move(rhs.m_payload);
  return *this;
}

auto Token::operator==(const Token& rhs) const noexcept -> bool {
  if (m_kind != rhs.m_kind) {
    return false;
  }
  if ((m_payload == nullptr) != (rhs.m_payload == nullptr)) {
    return false;
  }
  return m_payload == nullptr || *m_payload == *rhs.m_payload;
}

auto Token::operator!=(const Token& rhs) const noexcept -> bool { return !Token::operator==(rhs); }

auto Token::str() const -> std::string {
  std::ostringstream oss;
  oss << *this;
  return oss.str();
}

auto operator<<(std::ostream& out, const Token& rhs) -> std::ostream& {
  out << rhs.getKind();
  const auto payload = rhs.getPayload();
  if (payload) {
    out << '-' << *payload;
  }
  return out;
}

auto endToken() -> Token { return endToken(input::Span{0}); }

auto endToken(const input::Span span) -> Token { return Token{TokenKind::End, nullptr, span}; }

auto basicToken(const TokenKind kind, const input::Span span) -> Token {
  return Token{kind, nullptr, span};
}

auto errorToken(std::string msg, const input::Span span) -> Token {
  return Token{TokenKind::Error, std::make_unique<ErrorTokenPayload>(std::move(msg)), span};
}

auto litIntToken(const int32_t val, const input::Span span) -> Token {
  return Token{TokenKind::LitInt, std::make_unique<LitIntTokenPayload>(val), span};
}

auto litLongToken(const int64_t val, const input::Span span) -> Token {
  return Token{TokenKind::LitLong, std::make_unique<LitLongTokenPayload>(val), span};
}

auto litFloatToken(const float val, const input::Span span) -> Token {
  return Token{TokenKind::LitFloat, std::make_unique<LitFloatTokenPayload>(val), span};
}

auto litBoolToken(const bool val, const input::Span span) -> Token {
  return Token{TokenKind::LitBool, std::make_unique<LitBoolTokenPayload>(val), span};
}

auto litStrToken(std::string val, const input::Span span) -> Token {
  return Token{TokenKind::LitString, std::make_unique<LitStringTokenPayload>(std::move(val)), span};
}

auto litCharToken(uint8_t val, const input::Span span) -> Token {
  return Token{TokenKind::LitChar, std::make_unique<LitCharTokenPayload>(val), span};
}

auto keywordToken(Keyword keyword, const input::Span span) -> Token {
  return Token{TokenKind::Keyword, std::make_unique<KeywordTokenPayload>(keyword), span};
}

auto identiferToken(std::string id, const input::Span span) -> Token {
  return Token{
      TokenKind::Identifier, std::make_unique<IdentifierTokenPayload>(std::move(id)), span};
}

auto lineCommentToken(std::string comment, const input::Span span) -> Token {
  return Token{
      TokenKind::LineComment, std::make_unique<CommentTokenPayload>(std::move(comment)), span};
}

} // namespace lex
