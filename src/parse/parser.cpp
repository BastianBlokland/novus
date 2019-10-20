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
#include "parse/node_expr_call.hpp"
#include "parse/node_expr_const.hpp"
#include "parse/node_expr_const_decl.hpp"
#include "parse/node_expr_group.hpp"
#include "parse/node_expr_lit.hpp"
#include "parse/node_expr_paren.hpp"
#include "parse/node_expr_switch.hpp"
#include "parse/node_expr_switch_else.hpp"
#include "parse/node_expr_switch_if.hpp"
#include "parse/node_expr_unary.hpp"
#include "parse/node_stmt_print.hpp"
#include "parse/utilities.hpp"
#include <memory>
#include <stdexcept>
#include <vector>

namespace parse {

namespace internal {

auto ParserImpl::next() -> NodePtr { return nextStmt(); }

auto ParserImpl::nextStmt() -> NodePtr {
  if (peekToken(0).isEnd()) {
    return nullptr;
  }
  auto token = consumeToken();
  auto kwOpt = getKw(token);
  if (kwOpt && kwOpt.value() == lex::Keyword::Print) {
    return printStmtNode(token, nextExpr(0));
  }
  return errInvalidStmtStart(token);
}

auto ParserImpl::nextExpr() -> NodePtr {
  if (peekToken(0).isEnd()) {
    return nullptr;
  }
  return nextExpr(0);
}

auto ParserImpl::nextExpr(const int minPrecedence) -> NodePtr {
  auto lhs = nextExprLhs();
  while (true) {
    // Handle binary operators, precedence controls if we should keep recursing or let the next
    // iteration handle them.
    const auto& nextToken    = peekToken(0);
    const auto binPrecedence = getBinaryOpPrecedence(nextToken);
    if (binPrecedence == 0 || binPrecedence <= minPrecedence) {
      break;
    }

    if (nextToken.getType() == lex::TokenType::OpSemi) {
      lhs = nextExprGroup(std::move(lhs), binPrecedence);
    } else {
      auto op  = consumeToken();
      auto rhs = nextExpr(binPrecedence);
      lhs      = binaryExprNode(std::move(lhs), op, std::move(rhs));
    }
  }
  return lhs;
}

auto ParserImpl::nextExprLhs() -> NodePtr {
  const auto& nextToken = peekToken(0);
  if (nextToken.getCat() == lex::TokenCat::Operator) {
    const auto opToken    = consumeToken();
    const auto precedence = getUnaryOpPrecedence(opToken);
    if (precedence == 0) {
      return errInvalidUnaryOp(opToken, nextExpr(precedence));
    }
    return unaryExprNode(opToken, nextExpr(precedence));
  }

  return nextExprPrimary();
}

auto ParserImpl::nextExprGroup(NodePtr firstExpr, const int precedence) -> NodePtr {
  auto subExprs = std::vector<NodePtr>{};
  auto semis    = std::vector<lex::Token>{};

  subExprs.push_back(std::move(firstExpr));
  while (peekToken(0).getType() == lex::TokenType::OpSemi) {
    semis.push_back(consumeToken());
    subExprs.push_back(nextExpr(precedence));
  }

  return groupExprNode(std::move(subExprs), std::move(semis));
}

auto ParserImpl::nextExprPrimary() -> NodePtr {
  auto nextTok = peekToken(0);
  switch (nextTok.getCat()) {
  case lex::TokenCat::Literal:
    return litExprNode(consumeToken());
  case lex::TokenCat::Identifier: {
    auto id = consumeToken();
    if (peekToken(0).getType() == lex::TokenType::OpEq) {
      auto eq = consumeToken();
      return constDeclExprNode(std::move(id), std::move(eq), nextExpr(assignmentPrecedence));
    }
    if (peekToken(0).getType() == lex::TokenType::SepOpenParen) {
      return nextExprCall(std::move(id));
    }
    return constExprNode(std::move(id));
  }
  case lex::TokenCat::Keyword:
    if (getKw(nextTok) == lex::Keyword::If) {
      return nextExprSwitch();
    }
    [[fallthrough]];
  default:
    if (nextTok.getType() == lex::TokenType::SepOpenParen) {
      return nextExprParen();
    }
    return errInvalidPrimaryExpr(consumeToken());
  }
}

auto ParserImpl::nextExprCall(lex::Token id) -> NodePtr {
  auto open   = consumeToken();
  auto args   = std::vector<NodePtr>{};
  auto commas = std::vector<lex::Token>{};
  while (peekToken(0).getType() != lex::TokenType::SepCloseParen && !peekToken(0).isEnd()) {
    args.push_back(nextExpr(0));
    if (peekToken(0).getType() == lex::TokenType::SepComma) {
      commas.push_back(consumeToken());
    }
  }
  auto close = consumeToken();

  if (open.getType() == lex::TokenType::SepOpenParen &&
      close.getType() == lex::TokenType::SepCloseParen &&
      commas.size() == (args.empty() ? 0 : args.size() - 1)) {
    return callExprNode(
        std::move(id), std::move(open), std::move(args), std::move(commas), std::move(close));
  }
  return errInvalidCallExpr(
      std::move(id), std::move(open), std::move(args), std::move(commas), std::move(close));
}

auto ParserImpl::nextExprParen() -> NodePtr {
  auto open  = consumeToken();
  auto expr  = nextExpr(0);
  auto close = consumeToken();

  if (open.getType() == lex::TokenType::SepOpenParen &&
      close.getType() == lex::TokenType::SepCloseParen) {
    return parenExprNode(open, std::move(expr), close);
  }
  return errInvalidParenExpr(open, std::move(expr), close);
}

auto ParserImpl::nextExprSwitch() -> NodePtr {
  auto ifClauses = std::vector<NodePtr>{};
  do {
    ifClauses.push_back(nextExprSwitchIf());
  } while (getKw(peekToken(0)) == lex::Keyword::If);
  return switchExprNode(std::move(ifClauses), nextExprSwitchElse());
}

auto ParserImpl::nextExprSwitchIf() -> NodePtr {
  auto kw    = consumeToken();
  auto cond  = nextExpr(0);
  auto arrow = consumeToken();
  auto expr  = nextExpr(0);

  if (getKw(kw) == lex::Keyword::If && arrow.getType() == lex::TokenType::SepArrow) {
    return switchExprIfNode(kw, std::move(cond), arrow, std::move(expr));
  }
  return errInvalidSwitchIf(kw, std::move(cond), arrow, std::move(expr));
}

auto ParserImpl::nextExprSwitchElse() -> NodePtr {
  auto kw    = consumeToken();
  auto arrow = consumeToken();
  auto expr  = nextExpr(0);

  if (getKw(kw) == lex::Keyword::Else && arrow.getType() == lex::TokenType::SepArrow) {
    return switchExprElseNode(kw, arrow, std::move(expr));
  }
  return errInvalidSwitchElse(kw, arrow, std::move(expr));
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
