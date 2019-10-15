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

} // namespace parse
