#pragma once
#include "lex/token.hpp"
#include "parse/argument_list_decl.hpp"
#include "parse/node.hpp"
#include "parse/node_itr.hpp"
#include "parse/ret_type_spec.hpp"
#include "parse/type.hpp"
#include "parse/type_substitution_list.hpp"
#include <deque>

namespace parse {

namespace internal {

class ParserImpl {
protected:
  ParserImpl() noexcept : m_readBuffer{}, m_exprRecursionDepth{0} {}
  ~ParserImpl() noexcept;

  auto next() -> NodePtr;
  auto nextStmt() -> NodePtr;
  auto nextExpr() -> NodePtr;

private:
  std::deque<lex::Token> m_readBuffer;
  size_t m_exprRecursionDepth;

  auto nextComment() -> NodePtr;
  auto nextImport() -> NodePtr;
  auto nextStmtFuncDecl() -> NodePtr;
  auto nextStmtStructDecl() -> NodePtr;
  auto nextStmtUnionDecl() -> NodePtr;
  auto nextStmtEnumDecl() -> NodePtr;
  auto nextStmtExec() -> NodePtr;

  auto nextExpr(int minPrecedence, int maxPrecedence = 99) -> NodePtr;
  auto nextExprLhs() -> NodePtr;
  auto nextExprGroup(NodePtr firstExpr, int precedence) -> NodePtr;
  auto nextExprPrimary() -> NodePtr;
  auto nextExprId() -> NodePtr;
  auto nextExprId(lex::Token idToken) -> NodePtr;
  auto nextExprIntrinsic() -> NodePtr;
  auto nextExprField(NodePtr lhs) -> NodePtr;
  auto nextExprIs(NodePtr lhs) -> NodePtr;
  auto nextExprCall(NodePtr lhs, std::vector<lex::Token> modifiers = {}) -> NodePtr;
  auto nextExprIndex(NodePtr lhs) -> NodePtr;
  auto nextExprConditional(NodePtr condExpr) -> NodePtr;
  auto nextExprParen() -> NodePtr;
  auto nextExprSwitch() -> NodePtr;
  auto nextExprSwitchIf() -> NodePtr;
  auto nextExprSwitchElse() -> NodePtr;
  auto nextExprAnonFunc(std::vector<lex::Token> modifiers) -> NodePtr;

  auto nextType() -> Type;
  auto nextTypeParamList() -> TypeParamList;
  auto nextTypeSubstitutionList() -> TypeSubstitutionList;
  auto nextArgDeclList() -> ArgumentListDecl;
  auto nextRetTypeSpec() -> RetTypeSpec;

  auto consumeToken() -> lex::Token;
  auto peekToken(size_t ahead) -> lex::Token&;

  virtual auto getFromInput() -> lex::Token = 0;
};

} // namespace internal

// Parser object.
// Takes a token iterator and provides a parse node iterator.
template <typename InputItrBegin, typename InputItrEnd>
class Parser final : private internal::ParserImpl {

  static_assert(
      std::is_same<typename std::iterator_traits<InputItrBegin>::value_type, lex::Token>::value,
      "Valuetype of input iterator has to be 'Token'");

public:
  Parser() = delete;
  Parser(InputItrBegin inputBegin, const InputItrEnd inputEnd) :
      m_input{inputBegin}, m_inputEnd{inputEnd}, m_lastTokenSpan{input::Span{0}} {}

  [[nodiscard]] auto next() -> NodePtr { return internal::ParserImpl::next(); }

  [[nodiscard]] auto nextStmt() -> NodePtr { return internal::ParserImpl::nextStmt(); }

  [[nodiscard]] auto nextExpr() -> NodePtr { return internal::ParserImpl::nextExpr(); }

  [[nodiscard]] auto begin() -> NodeItr<Parser> { return NodeItr{this}; }

  [[nodiscard]] auto end() -> NodeItr<Parser> { return NodeItr<Parser>{}; }

private:
  InputItrBegin m_input;
  const InputItrEnd m_inputEnd;
  input::Span m_lastTokenSpan;

  auto getFromInput() -> lex::Token override {
    if (m_input == m_inputEnd) {
      return lex::endToken(m_lastTokenSpan);
    }
    auto val = *m_input;
    ++m_input;
    m_lastTokenSpan = val.getSpan();
    return val;
  }
};

// Parse a list of nodes from a pair of token iterators.
template <typename InputItrBegin, typename InputItrEnd>
auto parseAll(InputItrBegin inputBegin, const InputItrEnd inputEnd) {
  auto parser = Parser{inputBegin, inputEnd};
  auto result = std::vector<NodePtr>{};
  for (auto nodeItr = parser.begin(); nodeItr != parser.end(); ++nodeItr) {
    result.push_back(*nodeItr);
  }
  return result;
}

} // namespace parse
