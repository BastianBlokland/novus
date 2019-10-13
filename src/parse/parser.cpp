#include "parse/parser.hpp"
#include "lex/keyword.hpp"
#include "lex/token.hpp"
#include "lex/token_cat.hpp"
#include "lex/token_payload.hpp"
#include "lex/token_type.hpp"
#include "op_precedence.hpp"
#include "parse/error.hpp"
#include "parse/node.hpp"
#include "parse/node_expr_binary.hpp"
#include "parse/node_expr_const.hpp"
#include "parse/node_expr_lit.hpp"
#include "parse/node_expr_unary.hpp"
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

static auto isOp(const lex::Token& token) { return token.getCat() == lex::TokenCat::Operator; }

auto ParserImpl::next() -> NodePtr { return peekToken(0).isEnd() ? nullptr : nextStmt(); }

auto ParserImpl::nextStmt() -> NodePtr {
  auto token = consumeToken();
  auto kwOpt = getKw(token);
  if (kwOpt && kwOpt.value() == lex::Keyword::Print) {
    return printStmtNode(token, nextExpr(0));
  }
  return errInvalidStmtStart(token);
}

auto ParserImpl::nextExpr(const int minPrecedence) -> NodePtr {
  auto lhs = nextExprLhs(minPrecedence);
  while (true) {
    const auto& nextToken = peekToken(0);
    if (!isOp(nextToken)) {
      break;
    }
    const auto binPrecedence = getBinaryOpPrecedence(nextToken);
    if (binPrecedence <= minPrecedence) {
      break;
    }
    auto op  = consumeToken();
    auto rhs = nextExpr(binPrecedence);
    lhs      = binaryExprNode(std::move(lhs), op, std::move(rhs));
  }
  return lhs;
}

auto ParserImpl::nextExprLhs(const int minPrecedence) -> NodePtr {
  const auto& nextToken = peekToken(0);
  if (isOp(nextToken)) {
    const auto unaryPrecedence = getUnaryOpPrecedence(nextToken);
    if (unaryPrecedence >= minPrecedence) {
      const auto opToken = consumeToken();
      return unaryExprNode(opToken, nextExpr(unaryPrecedence));
    }
  }
  return nextExprPrimary();
}

auto ParserImpl::nextExprPrimary() -> NodePtr {
  auto tok = consumeToken();
  switch (tok.getCat()) {
  case lex::TokenCat::Literal:
    return litExprNode(tok);
  case lex::TokenCat::Identifier:
    return constExprNode(tok);
  default:
    return errInvalidPrimaryExpr(tok);
  }
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
