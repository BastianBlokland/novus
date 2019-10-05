#include "lex/token.hpp"

namespace lex {

Token::Token() : m_type{TokenType::End}, m_payload{nullptr}, m_span{0} {}

Token::Token(const TokenType type, TokenPayload* payload, const SourceSpan span)
    : m_type{type}, m_payload{payload}, m_span{span} {}

Token::Token(const Token& rhs)
    : m_type{rhs.m_type}, m_payload{!rhs.m_payload ? nullptr : rhs.m_payload->Clone()},
      m_span{rhs.m_span} {}

Token::Token(Token&& rhs) noexcept
    : m_type{rhs.m_type}, m_payload{rhs.m_payload}, m_span{rhs.m_span} {
  // Remove payload from rhs as we 'stole' it.
  rhs.m_payload = nullptr;
}

auto Token::operator=(const Token& rhs) -> Token& {
  if (this == &rhs) {
    return *this;
  }

  m_type = rhs.m_type;
  m_span = rhs.m_span;

  // Make a copy of the payload.
  delete m_payload;
  m_payload = rhs.m_payload ? rhs.m_payload->Clone() : nullptr;
  return *this;
}

auto Token::operator=(Token&& rhs) noexcept -> Token& {
  if (this == &rhs) {
    return *this;
  }

  m_type = rhs.m_type;
  m_span = rhs.m_span;

  // Steal the payload.
  delete m_payload;
  m_payload = rhs.m_payload;
  rhs.m_payload = nullptr;
  return *this;
}

auto Token::operator==(const Token& rhs) const noexcept -> bool {
  if (m_type != rhs.m_type) {
    return false;
  }

  if ((m_payload == nullptr) != (rhs.m_payload == nullptr)) {
    return false;
  }

  return m_payload == nullptr || m_payload->operator==(*rhs.m_payload);
}

auto Token::operator!=(const Token& rhs) const noexcept -> bool { return !Token::operator==(rhs); }

Token::~Token() {
  if (m_payload) {
    delete m_payload;
    m_payload = nullptr;
  }
}

auto operator<<(std::ostream& out, const Token& rhs) -> std::ostream& {
  out << rhs.getType();
  const auto payload = rhs.getPayload();
  if (payload) {
    out << '-' << '\'' << *payload << '\'';
  }
  return out;
}

auto endToken(const SourceSpan span) -> Token { return Token{TokenType::End, nullptr, span}; }

auto basicToken(const TokenType type, const SourceSpan span) -> Token {
  return Token{type, nullptr, span};
}

auto errorToken(const std::string& msg, const SourceSpan span) -> Token {
  return Token{TokenType::Error, new ErrorTokenPayload{msg}, span};
}

auto litIntToken(const int32_t val, const SourceSpan span) -> Token {
  return Token{TokenType::LitInt, new LitIntTokenPayload{val}, span};
}

auto litBoolToken(const bool val, const SourceSpan span) -> Token {
  return Token{TokenType::LitBool, new LitBoolTokenPayload{val}, span};
}

auto litStrToken(const std::string& val, const SourceSpan span) -> Token {
  return Token{TokenType::LitStr, new LitStringTokenPayload{val}, span};
}

auto keywordToken(Keyword keyword, const SourceSpan span) -> Token {
  return Token{TokenType::Keyword, new KeywordTokenPayload{keyword}, span};
}

auto identiferToken(const std::string& id, const SourceSpan span) -> Token {
  return Token{TokenType::Identifier, new IdentifierTokenPayload{id}, span};
}

} // namespace lex
