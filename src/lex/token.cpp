#include "lex/token.hpp"

namespace lex {

Token::Token() : _type{TokenType::Start}, _span{0}, _payload{nullptr} {}

Token::Token(const TokenType type, const SourceSpan span, TokenPayload* payload)
    : _type{type}, _span{span}, _payload{payload} {}

Token::Token(const Token& rhs)
    : _type{rhs._type}, _span{rhs._span}, _payload{rhs._payload == nullptr
                                                       ? nullptr
                                                       : rhs._payload->Clone()} {}

Token::Token(Token&& rhs) noexcept : _type{rhs._type}, _span{rhs._span}, _payload{rhs._payload} {
  // Remove payload from rhs as we 'stole' it.
  rhs._payload = nullptr;
}

auto Token::operator=(const Token& rhs) -> Token& {
  if (this == &rhs) {
    return *this;
  }

  _type = rhs._type;
  _span = rhs._span;

  // Make a copy of the payload.
  delete _payload;
  _payload = rhs._payload == nullptr ? nullptr : rhs._payload->Clone();
  return *this;
}

auto Token::operator=(Token&& rhs) noexcept -> Token& {
  if (this == &rhs) {
    return *this;
  }

  _type = rhs._type;
  _span = rhs._span;

  // Steal the payload.
  delete _payload;
  _payload = rhs._payload;
  rhs._payload = nullptr;
  return *this;
}

Token::~Token() {
  if (_payload != nullptr) {
    delete _payload;
    _payload = nullptr;
  }
}

auto Token::startToken() -> Token { return Token{TokenType::Start, 0, nullptr}; }

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
  if (payload != nullptr) {
    out << '-' << '\'' << *payload << '\'';
  }
  return out;
}

} // namespace lex
