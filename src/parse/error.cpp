#include "parse/error.hpp"
#include "lex/token_payload_error.hpp"
#include "parse/node_error.hpp"
#include "utilities.hpp"
#include <sstream>

namespace parse {

static auto addTokens(const Type& type, std::vector<lex::Token>* tokens) -> void;

auto errLexError(lex::Token errToken) -> NodePtr {
  if (errToken.getKind() != lex::TokenKind::Error) {
    throw std::invalid_argument{"Given token is not an error-token"};
  }
  const auto msg = errToken.getPayload<lex::ErrorTokenPayload>()->getMessage();
  return errorNode(msg, std::move(errToken));
}

auto errMaxExprRecursionDepthReached(lex::Token token) -> NodePtr {
  std::ostringstream oss;
  oss << "Maximum expression recursion depth reached, token: '" << token << '\'';
  return errorNode(oss.str(), std::move(token));
}

static auto addTokens(const TypeParamList& paramList, std::vector<lex::Token>* tokens) -> void {
  tokens->push_back(paramList.getOpen());
  for (const auto& type : paramList.getTypes()) {
    addTokens(type, tokens);
  }
  for (const auto& comma : paramList.getCommas()) {
    tokens->push_back(comma);
  }
  tokens->push_back(paramList.getClose());
}

static auto addTokens(const Type& type, std::vector<lex::Token>* tokens) -> void {
  tokens->push_back(type.getId());
  if (type.getParamList()) {
    addTokens(*type.getParamList(), tokens);
  }
}

static auto addTokens(const TypeSubstitutionList& subList, std::vector<lex::Token>* tokens)
    -> void {
  tokens->push_back(subList.getOpen());
  for (const auto& subToken : subList.getSubs()) {
    tokens->push_back(subToken);
  }
  for (const auto& comma : subList.getCommas()) {
    tokens->push_back(comma);
  }
  tokens->push_back(subList.getClose());
}

static auto addTokens(const ArgumentListDecl& argList, std::vector<lex::Token>* tokens) -> void {
  tokens->push_back(argList.getOpen());
  for (const auto& argToken : argList.getArgs()) {
    addTokens(argToken.getType(), tokens);
    tokens->push_back(argToken.getIdentifier());
  }
  for (const auto& comma : argList.getCommas()) {
    tokens->push_back(comma);
  }
  tokens->push_back(argList.getClose());
}

static auto getError(std::ostream& out, const ArgumentListDecl& argList) -> void {
  if (argList.getOpen().getKind() != lex::TokenKind::SepOpenParen &&
      argList.getOpen().getKind() != lex::TokenKind::OpParenParen) {
    out << "Expected opening parentheses '(' but got: '" << argList.getOpen() << '\'';
  } else if (argList.getCommas().size() != (argList.getCount() == 0 ? 0 : argList.getCount() - 1)) {
    out << "Incorrect number of comma's ',' in function declaration";
  } else if (
      argList.getClose().getKind() != lex::TokenKind::SepCloseParen &&
      argList.getClose().getKind() != lex::TokenKind::OpParenParen) {
    out << "Expected closing parentheses ')' but got: '" << argList.getClose() << '\'';
  } else {
    out << "Invalid argument list";
  }
}

auto errInvalidStmtImport(lex::Token kw, lex::Token path) -> NodePtr {
  std::ostringstream oss;
  if (path.getKind() != lex::TokenKind::LitString) {
    oss << "Expected import path (string) but got: '" << path << '\'';
  } else {
    oss << "Invalid import statement";
  }

  auto tokens = std::vector<lex::Token>{};
  tokens.push_back(std::move(kw));
  tokens.push_back(std::move(path));

  return errorNode(oss.str(), std::move(tokens));
}

auto errInvalidStmtFuncDecl(
    lex::Token kw,
    lex::Token id,
    std::optional<TypeSubstitutionList> typeSubs,
    const ArgumentListDecl& argList,
    std::optional<RetTypeSpec> retType,
    NodePtr body) -> NodePtr {

  std::ostringstream oss;
  if (id.getKind() != lex::TokenKind::Identifier && id.getCat() != lex::TokenCat::Operator) {
    oss << "Expected function identifier but got: '" << id << '\'';
  } else if (typeSubs && !typeSubs->validate()) {
    oss << "Invalid type substitution parameters";
  } else if (!argList.validate()) {
    getError(oss, argList);
  } else if (retType && !retType->validate()) {
    oss << "Invalid return-type specification: '" << *retType << '\'';
  } else {
    oss << "Invalid function declaration";
  }

  auto tokens = std::vector<lex::Token>{};
  tokens.push_back(std::move(kw));
  tokens.push_back(std::move(id));
  if (typeSubs) {
    addTokens(*typeSubs, &tokens);
  }
  addTokens(argList, &tokens);
  if (retType) {
    tokens.push_back(retType->getArrow());
    addTokens(retType->getType(), &tokens);
  }

  auto nodes = std::vector<std::unique_ptr<Node>>{};
  nodes.push_back(std::move(body));

  return errorNode(oss.str(), std::move(tokens), std::move(nodes));
}

auto errInvalidAnonFuncExpr(
    std::vector<lex::Token> modifiers,
    lex::Token kw,
    const ArgumentListDecl& argList,
    std::optional<RetTypeSpec> retType,
    NodePtr body) -> NodePtr {

  std::ostringstream oss;
  if (getKw(kw) != lex::Keyword::Lambda) {
    oss << "Expected keyword 'lambda' but got: '" << kw << '\'';
  } else if (!argList.validate()) {
    getError(oss, argList);
  } else if (retType && !retType->validate()) {
    oss << "Invalid return-type specification: '" << *retType << '\'';
  } else {
    oss << "Invalid anonymous function";
  }

  auto tokens = std::vector<lex::Token>{};
  for (auto& mod : modifiers) {
    tokens.push_back(std::move(mod));
  }
  tokens.push_back(std::move(kw));
  addTokens(argList, &tokens);
  if (retType) {
    tokens.push_back(retType->getArrow());
    addTokens(retType->getType(), &tokens);
  }

  auto nodes = std::vector<std::unique_ptr<Node>>{};
  nodes.push_back(std::move(body));

  return errorNode(oss.str(), std::move(tokens), std::move(nodes));
}

auto errInvalidStmtStructDecl(
    lex::Token kw,
    lex::Token id,
    std::optional<TypeSubstitutionList> typeSubs,
    std::optional<lex::Token> eq,
    const std::vector<StructDeclStmtNode::FieldSpec>& fields,
    std::vector<lex::Token> commas) -> NodePtr {

  std::ostringstream oss;
  if (id.getKind() != lex::TokenKind::Identifier) {
    oss << "Expected struct identifier but got: '" << id << '\'';
  } else if (typeSubs && !typeSubs->validate()) {
    oss << "Invalid type substitution parameters";
  } else if (eq && eq->getKind() != lex::TokenKind::OpEq) {
    oss << "Expected equals-sign '=' but got: '" << *eq << '\'';
  } else if (eq && fields.empty()) {
    oss << "Expected at least one field after the equals-sign '=' sign";
  } else if (commas.size() != (fields.empty() ? 0 : fields.size() - 1)) {
    oss << "Incorrect number of comma's ',' in struct declaration";
  } else {
    oss << "Invalid struct declaration";
  }

  auto tokens = std::vector<lex::Token>{};
  tokens.push_back(std::move(kw));
  tokens.push_back(std::move(id));
  if (typeSubs) {
    addTokens(*typeSubs, &tokens);
  }
  if (eq) {
    tokens.push_back(std::move(*eq));
  }
  for (auto& field : fields) {
    tokens.push_back(field.getIdentifier());
    addTokens(field.getType(), &tokens);
  }
  for (auto& comma : commas) {
    tokens.push_back(std::move(comma));
  }

  return errorNode(oss.str(), std::move(tokens), {});
}

auto errInvalidStmtUnionDecl(
    lex::Token kw,
    lex::Token id,
    std::optional<TypeSubstitutionList> typeSubs,
    lex::Token eq,
    const std::vector<Type>& types,
    std::vector<lex::Token> commas) -> NodePtr {

  std::ostringstream oss;
  if (id.getKind() != lex::TokenKind::Identifier) {
    oss << "Expected union identifier but got: '" << id << '\'';
  } else if (typeSubs && !typeSubs->validate()) {
    oss << "Invalid type substitution parameters";
  } else if (eq.getKind() != lex::TokenKind::OpEq) {
    oss << "Expected equals-sign '=' but got: '" << eq << '\'';
  } else if (types.size() < 2) {
    oss << "Union declaration needs at least two types";
  } else if (commas.size() != types.size() - 1) {
    oss << "Incorrect number of comma's ',' in union declaration";
  } else {
    oss << "Invalid union declaration";
  }

  auto tokens = std::vector<lex::Token>{};
  tokens.push_back(std::move(kw));
  tokens.push_back(std::move(id));
  if (typeSubs) {
    addTokens(*typeSubs, &tokens);
  }
  tokens.push_back(std::move(eq));
  for (auto& type : types) {
    addTokens(type, &tokens);
  }
  for (auto& comma : commas) {
    tokens.push_back(std::move(comma));
  }

  return errorNode(oss.str(), std::move(tokens), {});
}

auto errInvalidStmtEnumDecl(
    lex::Token kw,
    lex::Token id,
    lex::Token eq,
    const std::vector<EnumDeclStmtNode::EntrySpec>& entries,
    std::vector<lex::Token> commas) -> NodePtr {
  std::ostringstream oss;
  if (id.getKind() != lex::TokenKind::Identifier) {
    oss << "Expected union identifier but got: '" << id << '\'';
  } else if (eq.getKind() != lex::TokenKind::OpEq) {
    oss << "Expected equals-sign '=' but got: '" << eq << '\'';
  } else if (entries.empty()) {
    oss << "Enum declaration needs at least one entry";
  } else if (!std::all_of(entries.begin(), entries.end(), [](const auto& entry) {
               return entry.validate();
             })) {
    oss << "Invalid entry in enum declaration";
  } else if (commas.size() != entries.size() - 1) {
    oss << "Incorrect number of comma's ',' in enum declaration";
  } else {
    oss << "Invalid enum declaration";
  }

  auto tokens = std::vector<lex::Token>{};
  tokens.push_back(std::move(kw));
  tokens.push_back(std::move(id));
  tokens.push_back(std::move(eq));
  for (auto& entry : entries) {
    tokens.push_back(entry.getIdentifier());
    if (entry.getValueSpec()) {
      tokens.push_back(entry.getValueSpec()->getColon());
      if (entry.getValueSpec()->getMinusToken()) {
        tokens.push_back(*entry.getValueSpec()->getMinusToken());
      }
      tokens.push_back(entry.getValueSpec()->getValueToken());
    }
  }
  for (auto& comma : commas) {
    tokens.push_back(std::move(comma));
  }

  return errorNode(oss.str(), std::move(tokens), {});
}

auto errInvalidPrimaryExpr(lex::Token token) -> NodePtr {
  if (token.isError()) {
    return errLexError(token);
  }
  std::ostringstream oss;
  if (token.isEnd()) {
    oss << "Missing primary expression";
  } else {
    oss << "Invalid primary expression: '" << token << '\'';
  }
  return errorNode(oss.str(), std::move(token));
}

auto errInvalidUnaryOp(lex::Token op, NodePtr rhs) -> NodePtr {
  std::ostringstream oss;
  oss << "Invalid unary operator: '" << op << '\'';
  return errorNode(oss.str(), std::move(op), std::move(rhs));
}

auto errInvalidParenExpr(lex::Token open, NodePtr expr, lex::Token close) -> NodePtr {
  std::ostringstream oss;
  if (open.getKind() != lex::TokenKind::SepOpenParen &&
      open.getKind() != lex::TokenKind::OpParenParen) {
    oss << "Expected opening parentheses '(' but got: '" << open << '\'';
  } else if (
      close.getKind() != lex::TokenKind::SepCloseParen &&
      close.getKind() != lex::TokenKind::OpParenParen) {
    oss << "Expected closing parentheses ')' but got: '" << close << '\'';
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

auto errInvalidFieldExpr(NodePtr lhs, lex::Token dot, lex::Token id) -> NodePtr {
  std::ostringstream oss;
  if (dot.getKind() != lex::TokenKind::OpDot) {
    oss << "Expected dot '.' but got: '" << dot << '\'';
  } else if (id.getKind() != lex::TokenKind::Identifier) {
    oss << "Expected field identifier but got: '" << id << '\'';
  } else {
    oss << "Invalid field expression";
  }

  auto tokens = std::vector<lex::Token>{};
  tokens.push_back(std::move(dot));
  tokens.push_back(std::move(id));

  auto subExprs = std::vector<std::unique_ptr<Node>>{};
  subExprs.push_back(std::move(lhs));

  return errorNode(oss.str(), std::move(tokens), std::move(subExprs));
}

auto errInvalidIdExpr(lex::Token id, std::optional<TypeParamList> typeParams) -> NodePtr {
  std::ostringstream oss;
  if (typeParams && !typeParams->validate()) {
    oss << "Invalid type parameters";
  } else {
    oss << "Invalid id expression";
  }

  auto tokens = std::vector<lex::Token>{};
  tokens.push_back(std::move(id));
  if (typeParams) {
    addTokens(*typeParams, &tokens);
  }

  return errorNode(oss.str(), std::move(tokens), {});
}

auto errInvalidIntrinsicExpr(lex::Token kw, lex::Token open, lex::Token intrinsic, lex::Token close)
    -> NodePtr {
  std::ostringstream oss;
  if (open.getKind() != lex::TokenKind::SepOpenCurly) {
    oss << "Expected opening curly-brace '{' but got: '" << open << '\'';
  } else if (intrinsic.getKind() != lex::TokenKind::Identifier) {
    oss << "Expected an identifier but got: '" << intrinsic << '\'';
  } else if (close.getKind() != lex::TokenKind::SepCloseCurly) {
    oss << "Expected closing curly-brace '}' but got: '" << close << '\'';
  } else {
    oss << "Invalid intrinsic";
  }

  auto tokens = std::vector<lex::Token>{};
  tokens.push_back(std::move(kw));
  tokens.push_back(std::move(open));
  tokens.push_back(std::move(intrinsic));
  tokens.push_back(std::move(close));

  return errorNode(oss.str(), std::move(tokens), {});
}

auto errInvalidIsExpr(NodePtr lhs, lex::Token kw, const Type& type, std::optional<lex::Token> id)
    -> NodePtr {
  std::ostringstream oss;
  if (!type.validate()) {
    oss << "Invalid type identifier: " << type;
  } else if (
      id && id->getKind() != lex::TokenKind::Identifier &&
      id->getKind() != lex::TokenKind::Discard) {
    oss << "Expected identifier or discard '_' but got: '" << *id << '\'';
  } else {
    if (getKw(kw) == lex::Keyword::Is) {
      oss << "Invalid 'is' expression";
    } else {
      oss << "Invalid 'as' expression";
    }
  }

  auto tokens = std::vector<lex::Token>{};
  tokens.push_back(std::move(kw));
  addTokens(type, &tokens);
  if (id) {
    tokens.push_back(*id);
  }

  auto subExprs = std::vector<std::unique_ptr<Node>>{};
  subExprs.push_back(std::move(lhs));

  return errorNode(oss.str(), std::move(tokens), std::move(subExprs));
}

auto errInvalidCallExpr(
    std::vector<lex::Token> modifiers,
    NodePtr lhs,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) -> NodePtr {

  std::ostringstream oss;
  if (commas.size() != (args.empty() ? 0 : args.size() - 1)) {
    oss << "Incorrect number of comma's ',' in call expression";
  } else if (
      open.getKind() != lex::TokenKind::SepOpenParen &&
      open.getKind() != lex::TokenKind::OpParenParen) {
    oss << "Expected opening parentheses '(' but got: '" << open << '\'';
  } else if (
      close.getKind() != lex::TokenKind::SepCloseParen &&
      close.getKind() != lex::TokenKind::OpParenParen) {
    oss << "Expected closing parentheses ')' but got: '" << close << '\'';
  } else {
    oss << "Invalid call expression";
  }

  auto tokens = std::vector<lex::Token>{};
  for (auto& mod : modifiers) {
    tokens.push_back(std::move(mod));
  }
  tokens.push_back(std::move(open));
  for (auto& comma : commas) {
    tokens.push_back(std::move(comma));
  }
  tokens.push_back(std::move(close));

  auto nodes = std::vector<NodePtr>{};
  nodes.push_back(std::move(lhs));
  for (auto& arg : args) {
    nodes.push_back(std::move(arg));
  }

  return errorNode(oss.str(), std::move(tokens), std::move(nodes));
}

auto errInvalidIndexExpr(
    NodePtr lhs,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) -> NodePtr {

  std::ostringstream oss;
  if (args.empty()) {
    oss << "No argument given to index expression";
  } else if (commas.size() != args.size() - 1) {
    oss << "Incorrect number of comma's ',' in index expression";
  } else if (open.getKind() != lex::TokenKind::SepOpenSquare) {
    oss << "Expected opening square-bracket '[' but got: '" << open << '\'';
  } else if (close.getKind() != lex::TokenKind::SepCloseSquare) {
    oss << "Expected closing square-bracket ']' but got: '" << close << '\'';
  } else {
    oss << "Invalid index expression";
  }

  auto tokens = std::vector<lex::Token>{};
  tokens.push_back(std::move(open));
  for (auto& comma : commas) {
    tokens.push_back(std::move(comma));
  }
  tokens.push_back(std::move(close));

  auto nodes = std::vector<NodePtr>{};
  nodes.push_back(std::move(lhs));
  for (auto& arg : args) {
    nodes.push_back(std::move(arg));
  }

  return errorNode(oss.str(), std::move(tokens), std::move(nodes));
}

auto errInvalidConditionalExpr(
    NodePtr cond, lex::Token qmark, NodePtr ifBranch, lex::Token colon, NodePtr elseBranch)
    -> NodePtr {

  std::ostringstream oss;
  if (qmark.getKind() != lex::TokenKind::OpQMark) {
    oss << "Expected question-mark '?' but got: '" << qmark << '\'';
  } else if (colon.getKind() != lex::TokenKind::SepColon) {
    oss << "Expected colon ':' but got: '" << colon << '\'';
  } else {
    oss << "Invalid conditional operator";
  }

  auto tokens = std::vector<lex::Token>{};
  tokens.push_back(std::move(qmark));
  tokens.push_back(std::move(colon));

  auto nodes = std::vector<NodePtr>{};
  nodes.push_back(std::move(cond));
  nodes.push_back(std::move(ifBranch));
  nodes.push_back(std::move(elseBranch));

  return errorNode(oss.str(), std::move(tokens), std::move(nodes));
}

auto errInvalidSwitchIf(lex::Token kw, NodePtr cond, lex::Token arrow, NodePtr rhs) -> NodePtr {
  std::ostringstream oss;
  if (getKw(kw) != lex::Keyword::If) {
    oss << "Expected keyword 'if' but got: '" << kw << '\'';
  } else if (arrow.getKind() != lex::TokenKind::SepArrow) {
    oss << "Expected arrow '->' but got: '" << arrow << '\'';
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
    oss << "Expected keyword 'else' but got: '" << kw << '\'';
  } else if (arrow.getKind() != lex::TokenKind::SepArrow) {
    oss << "Expected arrow '->' but got: '" << arrow << '\'';
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
