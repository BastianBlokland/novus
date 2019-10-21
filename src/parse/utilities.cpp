#include "utilities.hpp"
#include "lex/token_payload_id.hpp"
#include "lex/token_payload_keyword.hpp"
#include <algorithm>
#include <iterator>

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

auto getSpan(const std::vector<lex::Token>& tokens) -> std::optional<lex::SourceSpan> {
  auto spans = std::vector<lex::SourceSpan>{};
  spans.reserve(tokens.size());
  std::transform(
      tokens.begin(), tokens.end(), std::back_insert_iterator(spans), [](const lex::Token& t) {
        return t.getSpan();
      });
  return lex::SourceSpan::combine(spans.begin(), spans.end());
}

auto getSpan(const std::vector<NodePtr>& nodes) -> std::optional<lex::SourceSpan> {
  auto spans = std::vector<lex::SourceSpan>{};
  spans.reserve(nodes.size());
  std::transform(
      nodes.begin(), nodes.end(), std::back_insert_iterator(spans), [](const NodePtr& t) {
        return t->getSpan();
      });
  return lex::SourceSpan::combine(spans.begin(), spans.end());
}

auto nodesEqual(const std::vector<NodePtr>& a, const std::vector<NodePtr>& b) -> bool {
  return a.size() == b.size() &&
      std::equal(a.begin(), a.end(), a.begin(), [](const NodePtr& l, const NodePtr& r) {
           return *l == *r;
         });
}

auto anyNodeNull(const std::vector<NodePtr>& v) -> bool {
  return std::any_of(v.begin(), v.end(), [](const NodePtr& p) { return p == nullptr; });
}

} // namespace parse
