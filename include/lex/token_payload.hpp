#pragma once
#include "lex/keyword.hpp"
#include <iostream>
#include <string>

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

} // namespace lex
