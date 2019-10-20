#include "lex/token.hpp"
#include "char_escape.hpp"
#include <memory>
#include <sstream>
#include <utility>

namespace lex {

Token::Token() : m_type{TokenType::End}, m_payload{nullptr}, m_span{0} {}

Token::Token(const TokenType type, std::unique_ptr<TokenPayload> payload, const SourceSpan span) :
    m_type{type}, m_payload{std::move(payload)}, m_span{span} {}

Token::Token(const Token& rhs) :
    m_type{rhs.m_type},
    m_payload{!rhs.m_payload ? nullptr : rhs.m_payload->clone()},
    m_span{rhs.m_span} {}

Token::Token(Token&& rhs) noexcept :
    m_type{rhs.m_type}, m_payload{std::move(rhs.m_payload)}, m_span{rhs.m_span} {}

auto Token::operator=(const Token& rhs) -> Token& {
  if (this == &rhs) {
    return *this;
  }
  m_type    = rhs.m_type;
  m_span    = rhs.m_span;
  m_payload = rhs.m_payload ? rhs.m_payload->clone() : nullptr;
  return *this;
}

auto Token::operator=(Token&& rhs) noexcept -> Token& {
  if (this == &rhs) {
    return *this;
  }
  m_type    = rhs.m_type;
  m_span    = rhs.m_span;
  m_payload = std::move(rhs.m_payload);
  return *this;
}

auto Token::operator==(const Token& rhs) const noexcept -> bool {
  if (m_type != rhs.m_type) {
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
  out << rhs.getType();
  const auto payload = rhs.getPayload();
  if (payload) {
    out << '-' << *payload;
  }
  return out;
}

auto endToken() -> Token { return endToken(SourceSpan{0}); }

auto endToken(const SourceSpan span) -> Token { return Token{TokenType::End, nullptr, span}; }

auto basicToken(const TokenType type, const SourceSpan span) -> Token {
  return Token{type, nullptr, span};
}

auto errorToken(std::string msg, const SourceSpan span) -> Token {
  return Token{TokenType::Error, std::make_unique<ErrorTokenPayload>(std::move(msg)), span};
}

auto litIntToken(const int32_t val, const SourceSpan span) -> Token {
  return Token{TokenType::LitInt, std::make_unique<LitIntTokenPayload>(val), span};
}

auto litBoolToken(const bool val, const SourceSpan span) -> Token {
  return Token{TokenType::LitBool, std::make_unique<LitBoolTokenPayload>(val), span};
}

auto litStrToken(std::string val, const SourceSpan span) -> Token {
  return Token{TokenType::LitStr, std::make_unique<LitStringTokenPayload>(std::move(val)), span};
}

auto keywordToken(Keyword keyword, const SourceSpan span) -> Token {
  return Token{TokenType::Keyword, std::make_unique<KeywordTokenPayload>(keyword), span};
}

auto identiferToken(std::string id, const SourceSpan span) -> Token {
  return Token{
      TokenType::Identifier, std::make_unique<IdentifierTokenPayload>(std::move(id)), span};
}

} // namespace lex
