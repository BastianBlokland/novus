#pragma once
#include "lex/token_payload.hpp"

namespace lex {

class LitFloatTokenPayload final : public TokenPayload {
public:
  LitFloatTokenPayload() = delete;
  explicit LitFloatTokenPayload(const float val) : m_val{val} {}

  auto operator==(const TokenPayload& rhs) const noexcept -> bool override {
    const auto castedRhs = dynamic_cast<const LitFloatTokenPayload*>(&rhs);
    return castedRhs != nullptr && m_val == castedRhs->m_val;
  }

  auto operator!=(const TokenPayload& rhs) const noexcept -> bool override {
    return !LitFloatTokenPayload::operator==(rhs);
  }

  [[nodiscard]] auto clone() -> std::unique_ptr<TokenPayload> override {
    return std::make_unique<LitFloatTokenPayload>(*this);
  }

  [[nodiscard]] auto getValue() const noexcept { return m_val; }

private:
  const float m_val;

  auto print(std::ostream& out) const -> std::ostream& override { return out << m_val; }
};

} // namespace lex
