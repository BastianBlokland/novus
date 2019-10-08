#pragma once
#include "lex/keyword.hpp"
#include "lex/source_span.hpp"
#include "lex/token_cat.hpp"
#include "lex/token_type.hpp"
#include <iostream>
#include <string>
#include <utility>

namespace lex {

class TokenPayload {
public:
  [[nodiscard]] virtual auto Clone() -> TokenPayload* = 0;

  virtual ~TokenPayload() = default;

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

class ErrorTokenPayload final : public TokenPayload {
public:
  ErrorTokenPayload() = delete;
  explicit ErrorTokenPayload(std::string msg) : m_msg{std::move(msg)} {}

  auto operator==(const TokenPayload& rhs) const noexcept -> bool override {
    if (typeid(rhs) == typeid(*this)) {
      return m_msg == static_cast<const ErrorTokenPayload&>(rhs).m_msg;
    }
    return false;
  }

  auto operator!=(const TokenPayload& rhs) const noexcept -> bool override {
    return !ErrorTokenPayload::operator==(rhs);
  }

  [[nodiscard]] auto Clone() -> TokenPayload* override { return new ErrorTokenPayload{*this}; }

  [[nodiscard]] auto getMessage() const noexcept -> const std::string& { return m_msg; }

private:
  const std::string m_msg;

  auto print(std::ostream& out) const -> std::ostream& override { return out << m_msg; }
};

class LitIntTokenPayload final : public TokenPayload {
public:
  LitIntTokenPayload() = delete;
  explicit LitIntTokenPayload(const int32_t val) : m_val{val} {}

  auto operator==(const TokenPayload& rhs) const noexcept -> bool override {
    if (typeid(rhs) == typeid(*this)) {
      return m_val == static_cast<const LitIntTokenPayload&>(rhs).m_val;
    }
    return false;
  }

  auto operator!=(const TokenPayload& rhs) const noexcept -> bool override {
    return !LitIntTokenPayload::operator==(rhs);
  }

  [[nodiscard]] auto Clone() -> TokenPayload* override { return new LitIntTokenPayload{*this}; }

  [[nodiscard]] auto getValue() const noexcept { return m_val; }

private:
  const int32_t m_val;

  auto print(std::ostream& out) const -> std::ostream& override { return out << m_val; }
};

class LitBoolTokenPayload final : public TokenPayload {
public:
  LitBoolTokenPayload() = delete;
  explicit LitBoolTokenPayload(const bool val) : m_val{val} {}

  auto operator==(const TokenPayload& rhs) const noexcept -> bool override {
    if (typeid(rhs) == typeid(*this)) {
      return m_val == static_cast<const LitBoolTokenPayload&>(rhs).m_val;
    }
    return false;
  }

  auto operator!=(const TokenPayload& rhs) const noexcept -> bool override {
    return !LitBoolTokenPayload::operator==(rhs);
  }

  [[nodiscard]] auto Clone() -> TokenPayload* override { return new LitBoolTokenPayload{*this}; }

  [[nodiscard]] auto getValue() const noexcept { return m_val; }

private:
  const bool m_val;

  auto print(std::ostream& out) const -> std::ostream& override {
    return out << (m_val ? "true" : "false");
  }
};

class LitStringTokenPayload final : public TokenPayload {
public:
  LitStringTokenPayload() = delete;
  explicit LitStringTokenPayload(std::string val) : m_val{std::move(val)} {}

  auto operator==(const TokenPayload& rhs) const noexcept -> bool override {
    if (typeid(rhs) == typeid(*this)) {
      return m_val == static_cast<const LitStringTokenPayload&>(rhs).m_val;
    }
    return false;
  }

  auto operator!=(const TokenPayload& rhs) const noexcept -> bool override {
    return !LitStringTokenPayload::operator==(rhs);
  }

  [[nodiscard]] auto Clone() -> TokenPayload* override { return new LitStringTokenPayload{*this}; }

  [[nodiscard]] auto getValue() const noexcept -> const std::string& { return m_val; }

private:
  const std::string m_val;

  auto print(std::ostream& out) const -> std::ostream& override;
};

class KeywordTokenPayload final : public TokenPayload {
public:
  KeywordTokenPayload() = delete;
  explicit KeywordTokenPayload(const Keyword keyword) : m_kw{keyword} {}

  auto operator==(const TokenPayload& rhs) const noexcept -> bool override {
    if (typeid(rhs) == typeid(*this)) {
      return m_kw == static_cast<const KeywordTokenPayload&>(rhs).m_kw;
    }
    return false;
  }

  auto operator!=(const TokenPayload& rhs) const noexcept -> bool override {
    return !KeywordTokenPayload::operator==(rhs);
  }

  [[nodiscard]] auto Clone() -> TokenPayload* override { return new KeywordTokenPayload{*this}; }

  [[nodiscard]] auto getKeyword() const noexcept { return m_kw; }

private:
  const Keyword m_kw;

  auto print(std::ostream& out) const -> std::ostream& override { return out << m_kw; }
};

class IdentifierTokenPayload final : public TokenPayload {
public:
  IdentifierTokenPayload() = delete;
  explicit IdentifierTokenPayload(std::string id) : m_id{std::move(id)} {}

  auto operator==(const TokenPayload& rhs) const noexcept -> bool override {
    if (typeid(rhs) == typeid(*this)) {
      return m_id == static_cast<const IdentifierTokenPayload&>(rhs).m_id;
    }
    return false;
  }

  auto operator!=(const TokenPayload& rhs) const noexcept -> bool override {
    return !IdentifierTokenPayload::operator==(rhs);
  }

  [[nodiscard]] auto Clone() -> TokenPayload* override { return new IdentifierTokenPayload{*this}; }

  [[nodiscard]] auto getIdentifier() const noexcept -> const std::string& { return m_id; }

private:
  const std::string m_id;

  auto print(std::ostream& out) const -> std::ostream& override { return out << m_id; }
};

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
