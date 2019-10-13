#include "parse/parser.hpp"
#include "lex/lexer.hpp"
#include "lex/token.hpp"
#include "lex/token_itr.hpp"
#include "parse/error.hpp"
#include <deque>
#include <memory>
#include <vector>

namespace parse {

namespace internal {

auto ParserImpl::next() -> std::unique_ptr<Node> {
  auto token = consumeToken();
  return errInvalidStmtStart(token);
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
