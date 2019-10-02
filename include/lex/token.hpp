#pragma once
#include "lex/keyword.hpp"
#include "lex/sourcespan.hpp"
#include "lex/tokentype.hpp"
#include <iostream>
#include <string>

namespace lex {

class TokenPayload {
public:
  virtual TokenPayload* Clone() = 0;
  virtual ~TokenPayload() {}

  friend std::ostream& operator<<(std::ostream& out, const TokenPayload& rhs) {
    return rhs.print(out);
  }

private:
  virtual auto print(std::ostream& out) const -> std::ostream& = 0;
};

class Token final {
public:
  Token();
  Token(const Token& rhs);
  Token(Token&& rhs) noexcept;
  Token& operator=(const Token& rhs);
  Token& operator=(Token&& rhs) noexcept;
  ~Token();

  auto getType() const noexcept -> TokenType { return _type; }

  auto getSpan() const noexcept -> SourceSpan { return _span; }

  auto getPayload() const noexcept -> TokenPayload* { return _payload; }

  auto isEnd() const noexcept -> bool { return _type == TokenType::End; }

  // Static factories.
  static auto startToken() -> Token;

  static auto endToken(const SourceSpan span) -> Token;

  static auto basicToken(const TokenType type, const SourceSpan span) -> Token;

  static auto errorToken(const SourceSpan span, const std::string& msg) -> Token;

  static auto litIntToken(const SourceSpan span, const uint32_t val) -> Token;

  static auto litBoolToken(const SourceSpan span, const bool val) -> Token;

  static auto litStrToken(const SourceSpan span, const std::string& val) -> Token;

  static auto keywordToken(const SourceSpan span, const Keyword keyword) -> Token;

  static auto identiferToken(const SourceSpan span, const std::string& id) -> Token;

private:
  TokenType _type;
  SourceSpan _span;
  TokenPayload* _payload;

  Token(const TokenType type, const SourceSpan span, TokenPayload* payload);
};

std::ostream& operator<<(std::ostream& out, const Token& rhs);

class ErrorTokenPayload final : public TokenPayload {
public:
  ErrorTokenPayload() = delete;
  ErrorTokenPayload(const std::string& msg) : _msg{msg} {}

  auto Clone() -> TokenPayload* override { return new ErrorTokenPayload{*this}; }

  auto getMessage() const noexcept -> const std::string& { return _msg; }

private:
  const std::string _msg;

  auto print(std::ostream& out) const -> std::ostream& override { return out << _msg; }
};

class LitIntTokenPayload final : public TokenPayload {
public:
  LitIntTokenPayload() = delete;
  LitIntTokenPayload(const uint32_t val) : _val{val} {}

  auto Clone() -> TokenPayload* override { return new LitIntTokenPayload{*this}; }

  auto getValue() const noexcept -> const uint32_t { return _val; }

private:
  const uint32_t _val;

  auto print(std::ostream& out) const -> std::ostream& override { return out << _val; }
};

class LitBoolTokenPayload final : public TokenPayload {
public:
  LitBoolTokenPayload() = delete;
  LitBoolTokenPayload(const bool val) : _val{val} {}

  auto Clone() -> TokenPayload* override { return new LitBoolTokenPayload{*this}; }

  auto getValue() const noexcept -> const bool { return _val; }

private:
  const bool _val;

  auto print(std::ostream& out) const -> std::ostream& override {
    return out << (_val ? "true" : "false");
  }
};

class LitStringTokenPayload final : public TokenPayload {
public:
  LitStringTokenPayload() = delete;
  LitStringTokenPayload(const std::string& val) : _val{val} {}

  auto Clone() -> TokenPayload* override { return new LitStringTokenPayload{*this}; }

  auto getValue() const noexcept -> const std::string& { return _val; }

private:
  const std::string _val;

  auto print(std::ostream& out) const -> std::ostream& override { return out << _val; }
};

class KeywordTokenPayload final : public TokenPayload {
public:
  KeywordTokenPayload() = delete;
  KeywordTokenPayload(const Keyword keyword) : _kw{keyword} {}

  auto Clone() -> TokenPayload* override { return new KeywordTokenPayload{*this}; }

  auto getKeyword() const noexcept -> const Keyword { return _kw; }

private:
  const Keyword _kw;

  auto print(std::ostream& out) const -> std::ostream& override { return out << _kw; }
};

class IdentifierTokenPayload final : public TokenPayload {
public:
  IdentifierTokenPayload() = delete;
  IdentifierTokenPayload(const std::string& id) : _id{id} {}

  auto Clone() -> TokenPayload* override { return new IdentifierTokenPayload{*this}; }

  auto getIdentifier() const noexcept -> const std::string& { return _id; }

private:
  const std::string _id;

  auto print(std::ostream& out) const -> std::ostream& override { return out << _id; }
};

} // namespace lex
