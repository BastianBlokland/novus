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

  auto getType() const noexcept -> TokenType { return m_type; }

  auto getSpan() const noexcept -> SourceSpan { return m_span; }

  auto getPayload() const noexcept -> TokenPayload* { return m_payload; }

  auto isEnd() const noexcept -> bool { return m_type == TokenType::End; }

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
  TokenType m_type;
  SourceSpan m_span;
  TokenPayload* m_payload;

  Token(const TokenType type, const SourceSpan span, TokenPayload* payload);
};

std::ostream& operator<<(std::ostream& out, const Token& rhs);

class ErrorTokenPayload final : public TokenPayload {
public:
  ErrorTokenPayload() = delete;
  ErrorTokenPayload(const std::string& msg) : m_msg{msg} {}

  auto Clone() -> TokenPayload* override { return new ErrorTokenPayload{*this}; }

  auto getMessage() const noexcept -> const std::string& { return m_msg; }

private:
  const std::string m_msg;

  auto print(std::ostream& out) const -> std::ostream& override { return out << m_msg; }
};

class LitIntTokenPayload final : public TokenPayload {
public:
  LitIntTokenPayload() = delete;
  LitIntTokenPayload(const uint32_t val) : m_val{val} {}

  auto Clone() -> TokenPayload* override { return new LitIntTokenPayload{*this}; }

  auto getValue() const noexcept -> const uint32_t { return m_val; }

private:
  const uint32_t m_val;

  auto print(std::ostream& out) const -> std::ostream& override { return out << m_val; }
};

class LitBoolTokenPayload final : public TokenPayload {
public:
  LitBoolTokenPayload() = delete;
  LitBoolTokenPayload(const bool val) : m_val{val} {}

  auto Clone() -> TokenPayload* override { return new LitBoolTokenPayload{*this}; }

  auto getValue() const noexcept -> const bool { return m_val; }

private:
  const bool m_val;

  auto print(std::ostream& out) const -> std::ostream& override {
    return out << (m_val ? "true" : "false");
  }
};

class LitStringTokenPayload final : public TokenPayload {
public:
  LitStringTokenPayload() = delete;
  LitStringTokenPayload(const std::string& val) : m_val{val} {}

  auto Clone() -> TokenPayload* override { return new LitStringTokenPayload{*this}; }

  auto getValue() const noexcept -> const std::string& { return m_val; }

private:
  const std::string m_val;

  auto print(std::ostream& out) const -> std::ostream& override { return out << m_val; }
};

class KeywordTokenPayload final : public TokenPayload {
public:
  KeywordTokenPayload() = delete;
  KeywordTokenPayload(const Keyword keyword) : m_kw{keyword} {}

  auto Clone() -> TokenPayload* override { return new KeywordTokenPayload{*this}; }

  auto getKeyword() const noexcept -> const Keyword { return m_kw; }

private:
  const Keyword m_kw;

  auto print(std::ostream& out) const -> std::ostream& override { return out << m_kw; }
};

class IdentifierTokenPayload final : public TokenPayload {
public:
  IdentifierTokenPayload() = delete;
  IdentifierTokenPayload(const std::string& id) : m_id{id} {}

  auto Clone() -> TokenPayload* override { return new IdentifierTokenPayload{*this}; }

  auto getIdentifier() const noexcept -> const std::string& { return m_id; }

private:
  const std::string m_id;

  auto print(std::ostream& out) const -> std::ostream& override { return out << m_id; }
};

} // namespace lex
