#include "parse/parser.hpp"
#include "lex/keyword.hpp"
#include "lex/token.hpp"
#include "lex/token_payload.hpp"
#include "lex/token_type.hpp"
#include "parse/error.hpp"
#include "parse/node_stmt_print.hpp"
#include <memory>

namespace parse {

namespace internal {

static auto getKw(const lex::Token& token) -> std::optional<lex::Keyword> {
  if (token.getType() != lex::TokenType::Keyword) {
    return std::nullopt;
  }
  return token.getPayload<lex::KeywordTokenPayload>()->getKeyword();
}

auto ParserImpl::next() -> std::unique_ptr<Node> {
  return peekToken(0).isEnd() ? nullptr : nextStmt();
}

auto ParserImpl::nextStmt() -> std::unique_ptr<Node> {
  auto token = consumeToken();
  auto kwOpt = getKw(token);
  if (kwOpt && kwOpt.value() == lex::Keyword::Print) {
    return printStmtNode(token, nextExpr());
  }
  return errInvalidStmtStart(token);
}

auto ParserImpl::nextExpr() -> std::unique_ptr<Node> {
  auto tok = consumeToken();
  return nullptr;
}

auto ParserImpl::consumeToken() -> lex::Token {
  if (!m_readBuffer.empty()) {
    const auto t = std::move(m_readBuffer.front());
    m_readBuffer.pop_front();
    return t;
  }
  return getFromInput();
}

auto ParserImpl::peekToken(const size_t ahead) -> lex::Token& {
  for (auto i = m_readBuffer.size(); i <= ahead; i++) {
    m_readBuffer.push_back(getFromInput());
  }
  return m_readBuffer[ahead];
}

} // namespace internal

// Explicit instantiations.
template class Parser<lex::Token*>;
template class Parser<std::vector<lex::Token>::iterator>;

} // namespace parse
