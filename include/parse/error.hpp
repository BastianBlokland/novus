#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/node_error.hpp"
#include <memory>
#include <string>

namespace parse {

auto errLexError(lex::Token errToken) -> NodePtr;

auto errInvalidStmtStart(lex::Token token) -> NodePtr;

auto errInvalidPrimaryExpr(lex::Token token) -> NodePtr;

auto errInvalidUnaryOp(lex::Token op, NodePtr rhs) -> NodePtr;

auto errInvalidParenExpr(lex::Token open, NodePtr expr, lex::Token close) -> NodePtr;

auto errInvalidCallExpr(
    lex::Token id,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) -> NodePtr;

auto errInvalidSwitchIf(lex::Token kw, NodePtr cond, lex::Token arrow, NodePtr rhs) -> NodePtr;

} // namespace parse
