#include "parse/parser.hpp"
#include "op_precedence.hpp"
#include "parse/error.hpp"
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
#include "parse/node_stmt_func_decl.hpp"
#include "parse/node_stmt_print.hpp"
#include "utilities.hpp"
#include <vector>

namespace parse {

namespace internal {

auto ParserImpl::next() -> NodePtr { return nextStmt(); }

auto ParserImpl::nextStmt() -> NodePtr {
  if (peekToken(0).isEnd()) {
    return nullptr;
  }
  const auto kw = getKw(peekToken(0));
  if (!kw) {
    return errInvalidStmt(consumeToken());
  }

  switch (kw.value()) {
  case lex::Keyword::Print:
    return nextStmtPrint();
  case lex::Keyword::Fun:
    return nextStmtFuncDecl();
  default:
    return errInvalidStmt(consumeToken());
  }
}

auto ParserImpl::nextExpr() -> NodePtr {
  if (peekToken(0).isEnd()) {
    return nullptr;
  }
  return nextExpr(0);
}

auto ParserImpl::nextStmtFuncDecl() -> NodePtr {
  auto kw     = consumeToken();
  auto id     = consumeToken();
  auto open   = consumeToken();
  auto args   = std::vector<FuncDeclStmtNode::arg>{};
  auto commas = std::vector<lex::Token>{};
  while (peekToken(0).getType() == lex::TokenKind::Identifier ||
         peekToken(0).getType() == lex::TokenKind::SepComma) {
    auto argType = consumeToken();
    auto argId   = consumeToken();
    args.emplace_back(argType, argId);
    if (peekToken(0).getType() == lex::TokenKind::SepComma) {
      commas.push_back(consumeToken());
    }
  }
  auto close   = consumeToken();
  auto arrow   = consumeToken();
  auto retType = consumeToken();
  auto body    = nextExpr(0);

  if (getKw(kw) == lex::Keyword::Fun && id.getType() == lex::TokenKind::Identifier &&
      open.getType() == lex::TokenKind::SepOpenParen &&
      close.getType() == lex::TokenKind::SepCloseParen &&
      arrow.getType() == lex::TokenKind::SepArrow &&
      retType.getType() == lex::TokenKind::Identifier &&
      std::all_of(
          args.begin(),
          args.end(),
          [](const auto& a) {
            return a.first.getType() == lex::TokenKind::Identifier &&
                a.second.getType() == lex::TokenKind::Identifier;
          }) &&
      commas.size() == (args.empty() ? 0 : args.size() - 1)) {

    return funcDeclStmtNode(
        std::move(kw),
        std::move(id),
        std::move(open),
        std::move(args),
        std::move(commas),
        std::move(close),
        std::move(arrow),
        std::move(retType),
        std::move(body));
  }
  return errInvalidStmtFuncDecl(
      std::move(kw),
      std::move(id),
      std::move(open),
      args,
      std::move(commas),
      std::move(close),
      std::move(arrow),
      std::move(retType),
      std::move(body));
}

auto ParserImpl::nextStmtPrint() -> NodePtr {
  auto kw   = consumeToken();
  auto body = nextExpr(0);
  if (getKw(kw) == lex::Keyword::Print) {
    return printStmtNode(kw, std::move(body));
  }
  return errInvalidStmtPrint(kw, std::move(body));
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

    if (nextToken.getType() == lex::TokenKind::OpSemi) {
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
  while (peekToken(0).getType() == lex::TokenKind::OpSemi) {
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
    if (peekToken(0).getType() == lex::TokenKind::OpEq) {
      auto eq = consumeToken();
      return constDeclExprNode(std::move(id), std::move(eq), nextExpr(assignmentPrecedence));
    }
    if (peekToken(0).getType() == lex::TokenKind::SepOpenParen) {
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
    if (nextTok.getType() == lex::TokenKind::SepOpenParen) {
      return nextExprParen();
    }
    return errInvalidPrimaryExpr(consumeToken());
  }
}

auto ParserImpl::nextExprCall(lex::Token id) -> NodePtr {
  auto open   = consumeToken();
  auto args   = std::vector<NodePtr>{};
  auto commas = std::vector<lex::Token>{};
  while (peekToken(0).getType() != lex::TokenKind::SepCloseParen && !peekToken(0).isEnd()) {
    args.push_back(nextExpr(0));
    if (peekToken(0).getType() == lex::TokenKind::SepComma) {
      commas.push_back(consumeToken());
    }
  }
  auto close = consumeToken();

  if (open.getType() == lex::TokenKind::SepOpenParen &&
      close.getType() == lex::TokenKind::SepCloseParen &&
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

  if (open.getType() == lex::TokenKind::SepOpenParen &&
      close.getType() == lex::TokenKind::SepCloseParen) {
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

  if (getKw(kw) == lex::Keyword::If && arrow.getType() == lex::TokenKind::SepArrow) {
    return switchExprIfNode(kw, std::move(cond), arrow, std::move(expr));
  }
  return errInvalidSwitchIf(kw, std::move(cond), arrow, std::move(expr));
}

auto ParserImpl::nextExprSwitchElse() -> NodePtr {
  auto kw    = consumeToken();
  auto arrow = consumeToken();
  auto expr  = nextExpr(0);

  if (getKw(kw) == lex::Keyword::Else && arrow.getType() == lex::TokenKind::SepArrow) {
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
