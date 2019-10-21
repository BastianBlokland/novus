#pragma once
#include "lex/token_payload.hpp"

namespace lex {

class LitStringTokenPayload final : public TokenPayload {
public:
  LitStringTokenPayload() = delete;
  explicit LitStringTokenPayload(std::string val) : m_val{std::move(val)} {}

  auto operator==(const TokenPayload& rhs) const noexcept -> bool override {
    const auto castedRhs = dynamic_cast<const LitStringTokenPayload*>(&rhs);
    return castedRhs != nullptr && m_val == castedRhs->m_val;
  }

  auto operator!=(const TokenPayload& rhs) const noexcept -> bool override {
    return !LitStringTokenPayload::operator==(rhs);
  }

  [[nodiscard]] auto clone() -> std::unique_ptr<TokenPayload> override {
    return std::make_unique<LitStringTokenPayload>(*this);
  }

  [[nodiscard]] auto getValue() const noexcept -> const std::string& { return m_val; }

private:
  const std::string m_val;

  auto print(std::ostream& out) const -> std::ostream& override;
};

} // namespace lex
