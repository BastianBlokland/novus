#pragma once
#include "lex/token_payload.hpp"

namespace lex {

class IdentifierTokenPayload final : public TokenPayload {
public:
  IdentifierTokenPayload() = delete;
  explicit IdentifierTokenPayload(std::string id) : m_id{std::move(id)} {}

  auto operator==(const TokenPayload& rhs) const noexcept -> bool override {
    const auto castedRhs = dynamic_cast<const IdentifierTokenPayload*>(&rhs);
    return castedRhs != nullptr && m_id == castedRhs->m_id;
  }

  auto operator!=(const TokenPayload& rhs) const noexcept -> bool override {
    return !IdentifierTokenPayload::operator==(rhs);
  }

  [[nodiscard]] auto clone() -> std::unique_ptr<TokenPayload> override {
    return std::make_unique<IdentifierTokenPayload>(*this);
  }

  [[nodiscard]] auto getIdentifier() const noexcept -> const std::string& { return m_id; }

private:
  const std::string m_id;

  auto print(std::ostream& out) const -> std::ostream& override { return out << m_id; }
};

} // namespace lex
