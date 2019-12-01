#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/node_stmt_func_decl.hpp"
#include "parse/node_stmt_struct_decl.hpp"

namespace parse {

auto errLexError(lex::Token errToken) -> NodePtr;

auto errInvalidStmtFuncDecl(
    lex::Token kw,
    lex::Token id,
    lex::Token open,
    const std::vector<FuncDeclStmtNode::ArgSpec>& args,
    std::vector<lex::Token> commas,
    lex::Token close,
    std::optional<FuncDeclStmtNode::RetTypeSpec> retType,
    NodePtr body) -> NodePtr;

auto errInvalidStmtStructDecl(
    lex::Token kw,
    lex::Token id,
    lex::Token eq,
    const std::vector<StructDeclStmtNode::FieldSpec>& fields,
    std::vector<lex::Token> commas) -> NodePtr;

auto errInvalidStmtExec(
    lex::Token action,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) -> NodePtr;

auto errInvalidPrimaryExpr(lex::Token token) -> NodePtr;

auto errInvalidUnaryOp(lex::Token op, NodePtr rhs) -> NodePtr;

auto errInvalidParenExpr(lex::Token open, NodePtr expr, lex::Token close) -> NodePtr;

auto errInvalidFieldExpr(NodePtr lhs, lex::Token dot, lex::Token id) -> NodePtr;

auto errInvalidCallExpr(
    lex::Token func,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) -> NodePtr;

auto errInvalidConditionalExpr(
    NodePtr cond, lex::Token qmark, NodePtr ifBranch, lex::Token colon, NodePtr elseBranch)
    -> NodePtr;

auto errInvalidSwitchIf(lex::Token kw, NodePtr cond, lex::Token arrow, NodePtr rhs) -> NodePtr;

auto errInvalidSwitchElse(lex::Token kw, lex::Token arrow, NodePtr rhs) -> NodePtr;

} // namespace parse
