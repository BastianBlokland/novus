#include "utilities.hpp"
#include "lex/token_payload_id.hpp"
#include "lex/token_payload_keyword.hpp"
#include "lex/token_payload_lit_int.hpp"
#include <algorithm>
#include <iterator>

namespace parse {

auto getKw(const lex::Token& token) -> std::optional<lex::Keyword> {
  if (token.getKind() != lex::TokenKind::Keyword) {
    return std::nullopt;
  }
  return token.getPayload<lex::KeywordTokenPayload>()->getKeyword();
}

auto getId(const lex::Token& token) -> std::optional<std::string> {
  if (token.getKind() != lex::TokenKind::Identifier) {
    return std::nullopt;
  }
  return token.getPayload<lex::IdentifierTokenPayload>()->getIdentifier();
}

auto getInt(const lex::Token& token) -> std::optional<int32_t> {
  if (token.getKind() != lex::TokenKind::LitInt) {
    return std::nullopt;
  }
  return token.getPayload<lex::LitIntTokenPayload>()->getValue();
}

auto getSpan(const std::vector<lex::Token>& tokens) -> std::optional<input::Span> {
  auto spans = std::vector<input::Span>{};
  spans.reserve(tokens.size());
  std::transform(
      tokens.begin(), tokens.end(), std::back_insert_iterator(spans), [](const lex::Token& t) {
        return t.getSpan();
      });
  return input::Span::combine(spans.begin(), spans.end());
}

auto getSpan(const std::vector<NodePtr>& nodes) -> std::optional<input::Span> {
  auto spans = std::vector<input::Span>{};
  spans.reserve(nodes.size());
  std::transform(
      nodes.begin(), nodes.end(), std::back_insert_iterator(spans), [](const NodePtr& t) {
        return t->getSpan();
      });
  return input::Span::combine(spans.begin(), spans.end());
}

auto nodesEqual(const std::vector<NodePtr>& a, const std::vector<NodePtr>& b) -> bool {
  return a.size() == b.size() &&
      std::equal(a.begin(), a.end(), b.begin(), [](const NodePtr& l, const NodePtr& r) {
           return *l == *r;
         });
}

auto anyNodeNull(const std::vector<NodePtr>& v) -> bool {
  return std::any_of(v.begin(), v.end(), [](const NodePtr& p) { return p == nullptr; });
}

auto validateParentheses(const lex::Token& open, const lex::Token& close) -> bool {
  if (open.getKind() == lex::TokenKind::SepOpenParen &&
      close.getKind() == lex::TokenKind::SepCloseParen) {
    return true;
  }
  if (open.getKind() == lex::TokenKind::OpParenParen &&
      close.getKind() == lex::TokenKind::OpParenParen) {
    return true;
  }
  return false;
}

} // namespace parse
