#pragma once
#include "lex/token_payload.hpp"

namespace lex {

class ErrorTokenPayload final : public TokenPayload {
public:
  ErrorTokenPayload() = delete;
  explicit ErrorTokenPayload(std::string msg) : m_msg{std::move(msg)} {}

  auto operator==(const TokenPayload& rhs) const noexcept -> bool override {
    const auto castedRhs = dynamic_cast<const ErrorTokenPayload*>(&rhs);
    return castedRhs != nullptr && m_msg == castedRhs->m_msg;
  }

  auto operator!=(const TokenPayload& rhs) const noexcept -> bool override {
    return !ErrorTokenPayload::operator==(rhs);
  }

  [[nodiscard]] auto clone() -> std::unique_ptr<TokenPayload> override {
    return std::make_unique<ErrorTokenPayload>(*this);
  }

  [[nodiscard]] auto getMessage() const noexcept -> const std::string& { return m_msg; }

private:
  const std::string m_msg;

  auto print(std::ostream& out) const -> std::ostream& override { return out << m_msg; }
};

} // namespace lex
