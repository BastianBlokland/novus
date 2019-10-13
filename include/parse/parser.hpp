#pragma once
#include "lex/source_span.hpp"
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/node_itr.hpp"
#include <deque>
#include <memory>
#include <utility>

namespace parse {

namespace internal {

class ParserImpl {
protected:
  ParserImpl() : m_readBuffer{} {}

  auto next() -> NodePtr;

private:
  std::deque<lex::Token> m_readBuffer;

  auto nextStmt() -> NodePtr;
  auto nextExpr(int minPrecedence) -> NodePtr;
  auto nextExprLhs(int minPrecedence) -> NodePtr;
  auto nextExprPrimary() -> NodePtr;

  auto consumeToken() -> lex::Token;
  auto peekToken(size_t ahead) -> lex::Token&;

  virtual auto getFromInput() -> lex::Token = 0;
};

} // namespace internal

template <typename InputItr>
class Parser final : private internal::ParserImpl {

  static_assert(
      std::is_same<typename std::iterator_traits<InputItr>::value_type, lex::Token>::value,
      "Valuetype of input iterator has to be 'Token'");

public:
  Parser() = delete;
  Parser(InputItr inputBegin, const InputItr inputEnd) :
      m_input{inputBegin}, m_inputEnd(inputEnd) {}

  auto next() -> NodePtr { return internal::ParserImpl::next(); }

  auto begin() -> NodeItr<Parser> { return NodeItr{*this}; }

  auto end() -> NodeItr<Parser> { return NodeItr<Parser>{}; }

private:
  InputItr m_input;
  const InputItr m_inputEnd;

  auto getFromInput() -> lex::Token override {
    if (m_input == m_inputEnd) {
      return lex::endToken();
    }
    return *m_input++;
  }
};

// Utilities.
template <typename InputItr>
auto parseAll(InputItr inputBegin, const InputItr inputEnd) {
  auto parser = Parser{inputBegin, inputEnd};
  return std::vector<NodePtr>{parser.begin(), parser.end()};
}

} // namespace parse
