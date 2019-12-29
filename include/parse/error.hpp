#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/node_stmt_func_decl.hpp"
#include "parse/node_stmt_struct_decl.hpp"
#include "parse/type_param_list.hpp"
#include "parse/type_substitution_list.hpp"

namespace parse {

auto errLexError(lex::Token errToken) -> NodePtr;

auto errInvalidStmtFuncDecl(
    lex::Token kw,
    lex::Token id,
    std::optional<TypeSubstitutionList> typeSubs,
    lex::Token open,
    const std::vector<FuncDeclStmtNode::ArgSpec>& args,
    std::vector<lex::Token> commas,
    lex::Token close,
    std::optional<FuncDeclStmtNode::RetTypeSpec> retType,
    NodePtr body) -> NodePtr;

auto errInvalidStmtStructDecl(
    lex::Token kw,
    lex::Token id,
    std::optional<TypeSubstitutionList> typeSubs,
    std::optional<lex::Token> eq,
    const std::vector<StructDeclStmtNode::FieldSpec>& fields,
    std::vector<lex::Token> commas) -> NodePtr;

auto errInvalidStmtUnionDecl(
    lex::Token kw,
    lex::Token id,
    std::optional<TypeSubstitutionList> typeSubs,
    lex::Token eq,
    const std::vector<Type>& types,
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

auto errInvalidIsExpr(NodePtr lhs, lex::Token kw, const Type& type, lex::Token id) -> NodePtr;

auto errInvalidCallExpr(
    NodePtr lhs,
    std::optional<TypeParamList> typeParams,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) -> NodePtr;

auto errInvalidIndexExpr(
    NodePtr lhs,
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
