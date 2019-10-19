#include "parse/error.hpp"
#include "lex/keyword.hpp"
#include "lex/token.hpp"
#include "lex/token_payload.hpp"
#include "lex/token_type.hpp"
#include "parse/node.hpp"
#include "parse/node_error.hpp"
#include <sstream>
#include <string>
#include <vector>

namespace parse {

auto errLexError(lex::Token errToken) -> NodePtr {
  if (errToken.getType() != lex::TokenType::Error) {
    throw std::invalid_argument("Given token is not an error-token");
  }
  const auto msg = errToken.getPayload<lex::ErrorTokenPayload>()->getMessage();
  return errorNode(msg, std::move(errToken));
}

auto errInvalidStmtStart(lex::Token token) -> NodePtr {
  if (token.isError()) {
    return errLexError(token);
  }
  std::ostringstream oss;
  oss << "Invalid statement start: " << token;
  return errorNode(oss.str(), std::move(token));
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
  if (open.getType() != lex::TokenType::SepOpenParen) {
    oss << "Expected opening parentheses but got: " << open;
  } else if (close.getType() != lex::TokenType::SepCloseParen) {
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
    lex::Token id,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) -> NodePtr {

  std::ostringstream oss;
  if (commas.size() != (args.empty() ? 0 : args.size() - 1)) {
    oss << "Incorrect number of comma's in call expression";
  } else if (open.getType() != lex::TokenType::SepOpenParen) {
    oss << "Expected opening parentheses but got: " << open;
  } else if (close.getType() != lex::TokenType::SepCloseParen) {
    oss << "Expected closing parentheses but got: " << close;
  } else {
    oss << "Invalid call expression";
  }

  auto tokens = std::vector<lex::Token>{};
  tokens.push_back(std::move(id));
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
  } else if (arrow.getType() != lex::TokenType::SepArrow) {
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
  } else if (arrow.getType() != lex::TokenType::SepArrow) {
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
