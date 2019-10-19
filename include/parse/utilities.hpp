#pragma once
#include "lex/token.hpp"
#include <algorithm>
#include <optional>

namespace parse {

template <typename Container>
auto cloneSet(const Container& cont) -> Container {
  auto result = Container{};
  std::transform(cont.begin(), cont.end(), std::back_inserter(result), [](const auto& n) {
    return n->clone();
  });
  return result;
}

inline auto getKw(const lex::Token& token) -> std::optional<lex::Keyword> {
  if (token.getType() != lex::TokenType::Keyword) {
    return std::nullopt;
  }
  return token.getPayload<lex::KeywordTokenPayload>()->getKeyword();
}

} // namespace parse
