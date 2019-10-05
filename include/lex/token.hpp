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

  virtual auto operator==(const TokenPayload& rhs) const noexcept -> bool = 0;
  virtual auto operator!=(const TokenPayload& rhs) const noexcept -> bool = 0;

  friend auto operator<<(std::ostream& out, const TokenPayload& rhs) -> std::ostream& {
    return rhs.print(out);
  }

private:
  virtual auto print(std::ostream& out) const -> std::ostream& = 0;
};

class Token final {
public:
  Token();
  Token(const TokenType type, const SourceSpan span, TokenPayload* payload);
  Token(const Token& rhs);
  Token(Token&& rhs) noexcept;

  auto operator=(const Token& rhs) -> Token&;
  auto operator=(Token&& rhs) noexcept -> Token&;

  auto operator==(const Token& rhs) noexcept -> bool;
  auto operator!=(const Token& rhs) noexcept -> bool;

  ~Token();

  auto getType() const noexcept { return m_type; }

  auto getSpan() const noexcept { return m_span; }

  auto getPayload() const noexcept { return m_payload; }

  auto isEnd() const noexcept { return m_type == TokenType::End; }

private:
  TokenType m_type;
  SourceSpan m_span;
  TokenPayload* m_payload;
};

std::ostream& operator<<(std::ostream& out, const Token& rhs);

class ErrorTokenPayload final : public TokenPayload {
public:
  ErrorTokenPayload() = delete;
  ErrorTokenPayload(const std::string& msg) : m_msg{msg} {}

  auto operator==(const TokenPayload& rhs) const noexcept -> bool override {
    if (typeid(rhs) == typeid(*this)) {
      return m_msg == static_cast<const ErrorTokenPayload&>(rhs).m_msg;
    }
    return false;
  }

  auto operator!=(const TokenPayload& rhs) const noexcept -> bool override {
    return !ErrorTokenPayload::operator==(rhs);
  }

  auto Clone() -> TokenPayload* override { return new ErrorTokenPayload{*this}; }

  auto getMessage() const noexcept -> const std::string& { return m_msg; }

private:
  const std::string m_msg;

  auto print(std::ostream& out) const -> std::ostream& override { return out << m_msg; }
};

class LitIntTokenPayload final : public TokenPayload {
public:
  LitIntTokenPayload() = delete;
  LitIntTokenPayload(const int32_t val) : m_val{val} {}

  auto operator==(const TokenPayload& rhs) const noexcept -> bool override {
    if (typeid(rhs) == typeid(*this)) {
      return m_val == static_cast<const LitIntTokenPayload&>(rhs).m_val;
    }
    return false;
  }

  auto operator!=(const TokenPayload& rhs) const noexcept -> bool override {
    return !LitIntTokenPayload::operator==(rhs);
  }

  auto Clone() -> TokenPayload* override { return new LitIntTokenPayload{*this}; }

  auto getValue() const noexcept { return m_val; }

private:
  const int32_t m_val;

  auto print(std::ostream& out) const -> std::ostream& override { return out << m_val; }
};

class LitBoolTokenPayload final : public TokenPayload {
public:
  LitBoolTokenPayload() = delete;
  LitBoolTokenPayload(const bool val) : m_val{val} {}

  auto operator==(const TokenPayload& rhs) const noexcept -> bool override {
    if (typeid(rhs) == typeid(*this)) {
      return m_val == static_cast<const LitBoolTokenPayload&>(rhs).m_val;
    }
    return false;
  }

  auto operator!=(const TokenPayload& rhs) const noexcept -> bool override {
    return !LitBoolTokenPayload::operator==(rhs);
  }

  auto Clone() -> TokenPayload* override { return new LitBoolTokenPayload{*this}; }

  auto getValue() const noexcept { return m_val; }

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

  auto operator==(const TokenPayload& rhs) const noexcept -> bool override {
    if (typeid(rhs) == typeid(*this)) {
      return m_val == static_cast<const LitStringTokenPayload&>(rhs).m_val;
    }
    return false;
  }

  auto operator!=(const TokenPayload& rhs) const noexcept -> bool override {
    return !LitStringTokenPayload::operator==(rhs);
  }

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

  auto operator==(const TokenPayload& rhs) const noexcept -> bool override {
    if (typeid(rhs) == typeid(*this)) {
      return m_kw == static_cast<const KeywordTokenPayload&>(rhs).m_kw;
    }
    return false;
  }

  auto operator!=(const TokenPayload& rhs) const noexcept -> bool override {
    return !KeywordTokenPayload::operator==(rhs);
  }

  auto Clone() -> TokenPayload* override { return new KeywordTokenPayload{*this}; }

  auto getKeyword() const noexcept { return m_kw; }

private:
  const Keyword m_kw;

  auto print(std::ostream& out) const -> std::ostream& override { return out << m_kw; }
};

class IdentifierTokenPayload final : public TokenPayload {
public:
  IdentifierTokenPayload() = delete;
  IdentifierTokenPayload(const std::string& id) : m_id{id} {}

  auto operator==(const TokenPayload& rhs) const noexcept -> bool override {
    if (typeid(rhs) == typeid(*this)) {
      return m_id == static_cast<const IdentifierTokenPayload&>(rhs).m_id;
    }
    return false;
  }

  auto operator!=(const TokenPayload& rhs) const noexcept -> bool override {
    return !IdentifierTokenPayload::operator==(rhs);
  }

  auto Clone() -> TokenPayload* override { return new IdentifierTokenPayload{*this}; }

  auto getIdentifier() const noexcept -> const std::string& { return m_id; }

private:
  const std::string m_id;

  auto print(std::ostream& out) const -> std::ostream& override { return out << m_id; }
};

// Factories.
auto endToken(const SourceSpan span) -> Token;

auto basicToken(const TokenType type, const SourceSpan span) -> Token;

auto errorToken(const SourceSpan span, const std::string& msg) -> Token;

auto litIntToken(const SourceSpan span, const int32_t val) -> Token;

auto litBoolToken(const SourceSpan span, const bool val) -> Token;

auto litStrToken(const SourceSpan span, const std::string& val) -> Token;

auto keywordToken(const SourceSpan span, const Keyword keyword) -> Token;

auto identiferToken(const SourceSpan span, const std::string& id) -> Token;

} // namespace lex
