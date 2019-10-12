#include "parse/parser.hpp"
#include "lex/lexer.hpp"
#include "lex/token.hpp"
#include "lex/token_itr.hpp"
#include "parse/statement.hpp"
#include <deque>
#include <memory>
#include <vector>

namespace parse {

template <typename InputItr>
auto Parser<InputItr>::next() -> std::unique_ptr<Statement> {
  return nullptr;
}

template <typename InputItr>
auto Parser<InputItr>::consumeToken() -> Token {
  if (!m_readBuffer.empty()) {
    const auto t = std::move(m_readBuffer.front());
    m_readBuffer.pop_front();
    return t;
  }
  return getFromInput();
}

template <typename InputItr>
auto Parser<InputItr>::peekToken(const size_t ahead) -> Token& {
  for (auto i = m_readBuffer.size(); i <= ahead; i++) {
    m_readBuffer.push_back(getFromInput());
  }
  return m_readBuffer[ahead];
}

template <typename InputItr>
auto Parser<InputItr>::getFromInput() -> Token {
  if (m_input == m_inputEnd) {
    return lex::endToken();
  }
  const auto val = *m_input;
  m_input++;
  return val;
}

// Explicit instantiations.
template class Parser<lex::TokenItr<lex::Lexer<char*>>>;
template class Parser<lex::TokenItr<lex::Lexer<std::string::iterator>>>;
template class Parser<lex::TokenItr<lex::Lexer<std::istream_iterator<char>>>>;
template class Parser<Token*>;
template class Parser<std::vector<Token>::iterator>;

} // namespace parse
z
