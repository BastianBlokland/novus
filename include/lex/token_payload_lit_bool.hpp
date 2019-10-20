#pragma once
#include "lex/token_payload.hpp"

namespace lex {

class LitBoolTokenPayload final : public TokenPayload {
public:
  LitBoolTokenPayload() = delete;
  explicit LitBoolTokenPayload(const bool val) : m_val{val} {}

  auto operator==(const TokenPayload& rhs) const noexcept -> bool override {
    const auto castedRhs = dynamic_cast<const LitBoolTokenPayload*>(&rhs);
    return castedRhs != nullptr && m_val == castedRhs->m_val;
  }

  auto operator!=(const TokenPayload& rhs) const noexcept -> bool override {
    return !LitBoolTokenPayload::operator==(rhs);
  }

  [[nodiscard]] auto clone() -> std::unique_ptr<TokenPayload> override {
    return std::make_unique<LitBoolTokenPayload>(*this);
  }

  [[nodiscard]] auto getValue() const noexcept { return m_val; }

private:
  const bool m_val;

  auto print(std::ostream& out) const -> std::ostream& override {
    return out << (m_val ? "true" : "false");
  }
};

} // namespace lex
