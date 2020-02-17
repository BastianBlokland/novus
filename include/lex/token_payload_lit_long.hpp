#pragma once
#include "lex/token_payload.hpp"

namespace lex {

class LitLongTokenPayload final : public TokenPayload {
public:
  LitLongTokenPayload() = delete;
  explicit LitLongTokenPayload(const int64_t val) : m_val{val} {}

  auto operator==(const TokenPayload& rhs) const noexcept -> bool override {
    const auto castedRhs = dynamic_cast<const LitLongTokenPayload*>(&rhs);
    return castedRhs != nullptr && m_val == castedRhs->m_val;
  }

  auto operator!=(const TokenPayload& rhs) const noexcept -> bool override {
    return !LitLongTokenPayload::operator==(rhs);
  }

  [[nodiscard]] auto clone() -> std::unique_ptr<TokenPayload> override {
    return std::make_unique<LitLongTokenPayload>(*this);
  }

  [[nodiscard]] auto getValue() const noexcept { return m_val; }

private:
  const int64_t m_val;

  auto print(std::ostream& out) const -> std::ostream& override { return out << m_val; }
};

} // namespace lex
