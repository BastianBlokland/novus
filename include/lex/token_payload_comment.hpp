#pragma once
#include "lex/token_payload.hpp"

namespace lex {

class CommentTokenPayload final : public TokenPayload {
public:
  CommentTokenPayload() = delete;
  explicit CommentTokenPayload(std::string comment) : m_comment{std::move(comment)} {}

  auto operator==(const TokenPayload& rhs) const noexcept -> bool override {
    const auto castedRhs = dynamic_cast<const CommentTokenPayload*>(&rhs);
    return castedRhs != nullptr && m_comment == castedRhs->m_comment;
  }

  auto operator!=(const TokenPayload& rhs) const noexcept -> bool override {
    return !CommentTokenPayload::operator==(rhs);
  }

  [[nodiscard]] auto clone() -> std::unique_ptr<TokenPayload> override {
    return std::make_unique<CommentTokenPayload>(*this);
  }

  [[nodiscard]] auto getValue() const noexcept -> const std::string& { return m_comment; }

private:
  const std::string m_comment;

  auto print(std::ostream& out) const -> std::ostream& override {
    return out << '\'' << m_comment << '\'';
  }
};

} // namespace lex
