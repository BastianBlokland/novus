#include "lex/token.hpp"

namespace lex {

Token::Token() : m_type{TokenType::End}, m_span{0}, m_payload{nullptr} {}

Token::Token(const TokenType type, const SourceSpan span, TokenPayload* payload)
    : m_type{type}, m_span{span}, m_payload{payload} {}

Token::Token(const Token& rhs)
    : m_type{rhs.m_type}, m_span{rhs.m_span}, m_payload{!rhs.m_payload ? nullptr
                                                                       : rhs.m_payload->Clone()} {}

Token::Token(Token&& rhs) noexcept
    : m_type{rhs.m_type}, m_span{rhs.m_span}, m_payload{rhs.m_payload} {
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

auto Token::operator==(const Token& rhs) noexcept -> bool {
  if (m_type != rhs.m_type) {
    return false;
  }

  if ((m_payload == nullptr) != (rhs.m_payload == nullptr)) {
    return false;
  }

  return m_payload == nullptr || m_payload->operator==(*rhs.m_payload);
}

auto Token::operator!=(const Token& rhs) noexcept -> bool { return !Token::operator==(rhs); }

Token::~Token() {
  if (m_payload) {
    delete m_payload;
    m_payload = nullptr;
  }
}

auto Token::endToken(const SourceSpan span) -> Token {
  return Token{TokenType::End, span, nullptr};
}

auto Token::basicToken(const TokenType type, const SourceSpan span) -> Token {
  return Token{type, span, nullptr};
}

auto Token::errorToken(const SourceSpan span, const std::string& msg) -> Token {
  return Token{TokenType::Error, span, new ErrorTokenPayload{msg}};
}

auto Token::litIntToken(const SourceSpan span, const uint32_t val) -> Token {
  return Token{TokenType::LitInt, span, new LitIntTokenPayload{val}};
}

auto Token::litBoolToken(const SourceSpan span, const bool val) -> Token {
  return Token{TokenType::LitBool, span, new LitBoolTokenPayload{val}};
}

auto Token::litStrToken(const SourceSpan span, const std::string& val) -> Token {
  return Token{TokenType::LitStr, span, new LitStringTokenPayload{val}};
}

auto Token::keywordToken(const SourceSpan span, Keyword keyword) -> Token {
  return Token{TokenType::Keyword, span, new KeywordTokenPayload{keyword}};
}

auto Token::identiferToken(const SourceSpan span, const std::string& id) -> Token {
  return Token{TokenType::Identifier, span, new IdentifierTokenPayload{id}};
}

auto operator<<(std::ostream& out, const Token& rhs) -> std::ostream& {
  out << rhs.getType();
  const auto payload = rhs.getPayload();
  if (payload) {
    out << '-' << '\'' << *payload << '\'';
  }
  return out;
}

} // namespace lex
