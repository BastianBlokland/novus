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
  auto nextStmt() -> NodePtr;
  auto nextExpr() -> NodePtr;

private:
  std::deque<lex::Token> m_readBuffer;

  auto nextExpr(int minPrecedence) -> NodePtr;
  auto nextExprLhs() -> NodePtr;
  auto nextExprGroup(NodePtr firstExpr, int precedence) -> NodePtr;
  auto nextExprPrimary() -> NodePtr;
  auto nextExprCall(lex::Token id) -> NodePtr;
  auto nextExprParen() -> NodePtr;

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

  [[nodiscard]] auto next() -> NodePtr { return internal::ParserImpl::next(); }

  [[nodiscard]] auto nextStmt() -> NodePtr { return internal::ParserImpl::nextStmt(); }

  [[nodiscard]] auto nextExpr() -> NodePtr { return internal::ParserImpl::nextExpr(); }

  [[nodiscard]] auto begin() -> NodeItr<Parser> { return NodeItr{*this}; }

  [[nodiscard]] auto end() -> NodeItr<Parser> { return NodeItr<Parser>{}; }

private:
  InputItr m_input;
  const InputItr m_inputEnd;

  auto getFromInput() -> lex::Token override {
    if (m_input == m_inputEnd) {
      return lex::endToken();
    }
    auto val = *m_input;
    ++m_input;
    return std::move(val);
  }
};

// Utilities.
template <typename InputItr>
auto parseAll(InputItr inputBegin, const InputItr inputEnd) {
  auto parser = Parser{inputBegin, inputEnd};
  return std::vector<NodePtr>{parser.begin(), parser.end()};
}

} // namespace parse
