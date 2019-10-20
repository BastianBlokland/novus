#include "utilities.hpp"

namespace parse {

auto getKw(const lex::Token& token) -> std::optional<lex::Keyword> {
  if (token.getType() != lex::TokenType::Keyword) {
    return std::nullopt;
  }
  return token.getPayload<lex::KeywordTokenPayload>()->getKeyword();
}

auto getId(const lex::Token& token) -> std::optional<std::string> {
  if (token.getType() != lex::TokenType::Identifier) {
    return std::nullopt;
  }
  return token.getPayload<lex::IdentifierTokenPayload>()->getIdentifier();
}

} // namespace parse
