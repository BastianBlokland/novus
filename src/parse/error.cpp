#include "parse/error.hpp"
#include "lex/token_payload_error.hpp"
#include "parse/node_error.hpp"
#include "utilities.hpp"
#include <sstream>

namespace parse {

auto errLexError(lex::Token errToken) -> NodePtr {
  if (errToken.getKind() != lex::TokenKind::Error) {
    throw std::invalid_argument{"Given token is not an error-token"};
  }
  const auto msg = errToken.getPayload<lex::ErrorTokenPayload>()->getMessage();
  return errorNode(msg, std::move(errToken));
}

auto errInvalidStmtFuncDecl(
    lex::Token kw,
    lex::Token id,
    lex::Token open,
    const std::vector<FuncDeclStmtNode::arg>& args,
    std::vector<lex::Token> commas,
    lex::Token close,
    lex::Token arrow,
    lex::Token retType,
    NodePtr body) -> NodePtr {

  std::ostringstream oss;
  if (id.getKind() != lex::TokenKind::Identifier) {
    oss << "Expected function identifier but got: " << id;
  } else if (open.getKind() != lex::TokenKind::SepOpenParen) {
    oss << "Expected opening parentheses but got: " << open;
  } else if (commas.size() != (args.empty() ? 0 : args.size() - 1)) {
    oss << "Incorrect number of comma's in function declaration";
  } else if (close.getKind() != lex::TokenKind::SepCloseParen) {
    oss << "Expected closing parentheses but got: " << close;
  } else if (arrow.getKind() != lex::TokenKind::SepArrow) {
    oss << "Expected return-type seperator (->) but got: " << arrow;
  } else if (retType.getKind() != lex::TokenKind::Identifier) {
    oss << "Expected return-type identifier but got: " << retType;
  } else {
    oss << "Invalid function declaration";
  }

  auto tokens = std::vector<lex::Token>{};
  tokens.push_back(std::move(kw));
  tokens.push_back(std::move(id));
  tokens.push_back(std::move(open));
  for (auto& arg : args) {
    tokens.push_back(arg.first);
    tokens.push_back(arg.second);
  }
  for (auto& comma : commas) {
    tokens.push_back(std::move(comma));
  }
  tokens.push_back(std::move(close));
  tokens.push_back(std::move(arrow));
  tokens.push_back(std::move(retType));

  auto nodes = std::vector<std::unique_ptr<Node>>{};
  nodes.push_back(std::move(body));

  return errorNode(oss.str(), std::move(tokens), std::move(nodes));
}

auto errInvalidStmtExec(
    lex::Token action,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) -> NodePtr {

  std::ostringstream oss;
  if (open.getKind() != lex::TokenKind::SepOpenParen) {
    oss << "Expected opening parentheses but got: " << open;
  } else if (close.getKind() != lex::TokenKind::SepCloseParen) {
    oss << "Expected closing parentheses but got: " << close;
  } else if (commas.size() != (args.empty() ? 0 : args.size() - 1)) {
    oss << "Incorrect number of comma's in execute statement";
  } else {
    oss << "Invalid execute statement";
  }

  auto tokens = std::vector<lex::Token>{};
  tokens.push_back(std::move(action));
  tokens.push_back(std::move(open));
  for (auto& comma : commas) {
    tokens.push_back(std::move(comma));
  }
  tokens.push_back(std::move(close));

  return errorNode(oss.str(), std::move(tokens), std::move(args));
}

auto errInvalidPrimaryExpr(lex::Token token) -> NodePtr {
  if (token.isError()) {
    return errLexError(token);
  }
  std::ostringstream oss;
  if (token.isEnd()) {
    oss << "Missing primary expression";
  } else {
    oss << "Invalid primary expression: " << token;
  }
  return errorNode(oss.str(), std::move(token));
}

auto errInvalidUnaryOp(lex::Token op, NodePtr rhs) -> NodePtr {
  std::ostringstream oss;
  oss << "Invalid unary operator: " << op;
  return errorNode(oss.str(), std::move(op), std::move(rhs));
}

auto errInvalidParenExpr(lex::Token open, NodePtr expr, lex::Token close) -> NodePtr {
  std::ostringstream oss;
  if (open.getKind() != lex::TokenKind::SepOpenParen) {
    oss << "Expected opening parentheses but got: " << open;
  } else if (close.getKind() != lex::TokenKind::SepCloseParen) {
    oss << "Expected closing parentheses but got: " << close;
  } else {
    oss << "Invalid parenthesized expression";
  }

  auto tokens = std::vector<lex::Token>{};
  tokens.push_back(std::move(open));
  tokens.push_back(std::move(close));

  auto subExprs = std::vector<std::unique_ptr<Node>>{};
  subExprs.push_back(std::move(expr));

  return errorNode(oss.str(), std::move(tokens), std::move(subExprs));
}

auto errInvalidCallExpr(
    lex::Token func,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) -> NodePtr {

  std::ostringstream oss;
  if (commas.size() != (args.empty() ? 0 : args.size() - 1)) {
    oss << "Incorrect number of comma's in call expression";
  } else if (open.getKind() != lex::TokenKind::SepOpenParen) {
    oss << "Expected opening parentheses but got: " << open;
  } else if (close.getKind() != lex::TokenKind::SepCloseParen) {
    oss << "Expected closing parentheses but got: " << close;
  } else {
    oss << "Invalid call expression";
  }

  auto tokens = std::vector<lex::Token>{};
  tokens.push_back(std::move(func));
  tokens.push_back(std::move(open));
  for (auto& comma : commas) {
    tokens.push_back(std::move(comma));
  }
  tokens.push_back(std::move(close));

  return errorNode(oss.str(), std::move(tokens), std::move(args));
}

auto errInvalidSwitchIf(lex::Token kw, NodePtr cond, lex::Token arrow, NodePtr rhs) -> NodePtr {
  std::ostringstream oss;
  if (getKw(kw) != lex::Keyword::If) {
    oss << "Expected keyword 'if' but got: " << kw;
  } else if (arrow.getKind() != lex::TokenKind::SepArrow) {
    oss << "Expected arrow '->' but got: " << arrow;
  } else {
    oss << "Invalid if clause";
  }

  auto tokens = std::vector<lex::Token>{};
  tokens.push_back(std::move(kw));
  tokens.push_back(std::move(arrow));

  auto nodes = std::vector<NodePtr>{};
  nodes.push_back(std::move(cond));
  nodes.push_back(std::move(rhs));

  return errorNode(oss.str(), std::move(tokens), std::move(nodes));
}

auto errInvalidSwitchElse(lex::Token kw, lex::Token arrow, NodePtr rhs) -> NodePtr {
  std::ostringstream oss;
  if (getKw(kw) != lex::Keyword::Else) {
    oss << "Expected keyword 'else' but got: " << kw;
  } else if (arrow.getKind() != lex::TokenKind::SepArrow) {
    oss << "Expected arrow '->' but got: " << arrow;
  } else {
    oss << "Invalid else clause";
  }

  auto tokens = std::vector<lex::Token>{};
  tokens.push_back(std::move(kw));
  tokens.push_back(std::move(arrow));

  auto nodes = std::vector<NodePtr>{};
  nodes.push_back(std::move(rhs));

  return errorNode(oss.str(), std::move(tokens), std::move(nodes));
}

} // namespace parse
