#pragma once
#include "lex/token_payload.hpp"

namespace lex {

class KeywordTokenPayload final : public TokenPayload {
public:
  KeywordTokenPayload() = delete;
  explicit KeywordTokenPayload(const Keyword keyword) : m_kw{keyword} {}

  auto operator==(const TokenPayload& rhs) const noexcept -> bool override {
    const auto castedRhs = dynamic_cast<const KeywordTokenPayload*>(&rhs);
    return castedRhs != nullptr && m_kw == castedRhs->m_kw;
  }

  auto operator!=(const TokenPayload& rhs) const noexcept -> bool override {
    return !KeywordTokenPayload::operator==(rhs);
  }

  [[nodiscard]] auto clone() -> std::unique_ptr<TokenPayload> override {
    return std::make_unique<KeywordTokenPayload>(*this);
  }

  [[nodiscard]] auto getKeyword() const noexcept { return m_kw; }

private:
  const Keyword m_kw;

  auto print(std::ostream& out) const -> std::ostream& override { return out << m_kw; }
};

} // namespace lex
