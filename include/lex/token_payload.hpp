#pragma once
#include "lex/keyword.hpp"
#include <iostream>
#include <memory>
#include <string>

namespace lex {

class TokenPayload {
public:
  TokenPayload()                            = default;
  TokenPayload(const TokenPayload& rhs)     = default;
  TokenPayload(TokenPayload&& rhs) noexcept = default;
  virtual ~TokenPayload()                   = default;

  auto operator=(const TokenPayload& rhs) -> TokenPayload& = default;
  auto operator=(TokenPayload&& rhs) noexcept -> TokenPayload& = default;

  [[nodiscard]] virtual auto clone() -> std::unique_ptr<TokenPayload> = 0;

  virtual auto operator==(const TokenPayload& rhs) const noexcept -> bool = 0;
  virtual auto operator!=(const TokenPayload& rhs) const noexcept -> bool = 0;

  friend auto operator<<(std::ostream& out, const TokenPayload& rhs) -> std::ostream& {
    return rhs.print(out);
  }

private:
  virtual auto print(std::ostream& out) const -> std::ostream& = 0;
};

} // namespace lex
