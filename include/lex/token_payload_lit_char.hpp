#pragma once
#include "lex/token_payload.hpp"

namespace lex {

class LitCharTokenPayload final : public TokenPayload {
public:
  LitCharTokenPayload() = delete;
  explicit LitCharTokenPayload(const char val) : m_val{val} {}

  auto operator==(const TokenPayload& rhs) const noexcept -> bool override {
    const auto castedRhs = dynamic_cast<const LitCharTokenPayload*>(&rhs);
    return castedRhs != nullptr && m_val == castedRhs->m_val;
  }

  auto operator!=(const TokenPayload& rhs) const noexcept -> bool override {
    return !LitCharTokenPayload::operator==(rhs);
  }

  [[nodiscard]] auto clone() -> std::unique_ptr<TokenPayload> override {
    return std::make_unique<LitCharTokenPayload>(*this);
  }

  [[nodiscard]] auto getValue() const noexcept { return m_val; }

private:
  const char m_val;

  auto print(std::ostream& out) const -> std::ostream& override;
};

} // namespace lex
