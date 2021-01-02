#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/node_stmt_enum_decl.hpp"
#include "parse/node_stmt_func_decl.hpp"
#include "parse/node_stmt_struct_decl.hpp"
#include "parse/type_param_list.hpp"
#include "parse/type_substitution_list.hpp"

namespace parse {

[[nodiscard]] auto errLexError(lex::Token errToken) -> NodePtr;

[[nodiscard]] auto errMaxExprRecursionDepthReached(lex::Token token) -> NodePtr;

[[nodiscard]] auto errInvalidStmtImport(lex::Token kw, lex::Token path) -> NodePtr;

[[nodiscard]] auto errInvalidStmtFuncDecl(
    lex::Token kw,
    lex::Token id,
    std::optional<TypeSubstitutionList> typeSubs,
    const ArgumentListDecl& argList,
    std::optional<RetTypeSpec> retType,
    NodePtr body) -> NodePtr;

[[nodiscard]] auto errInvalidAnonFuncExpr(
    std::vector<lex::Token> modifiers,
    lex::Token kw,
    const ArgumentListDecl& argList,
    std::optional<RetTypeSpec> retType,
    NodePtr body) -> NodePtr;

[[nodiscard]] auto errInvalidStmtStructDecl(
    lex::Token kw,
    lex::Token id,
    std::optional<TypeSubstitutionList> typeSubs,
    std::optional<lex::Token> eq,
    const std::vector<StructDeclStmtNode::FieldSpec>& fields,
    std::vector<lex::Token> commas) -> NodePtr;

[[nodiscard]] auto errInvalidStmtUnionDecl(
    lex::Token kw,
    lex::Token id,
    std::optional<TypeSubstitutionList> typeSubs,
    lex::Token eq,
    const std::vector<Type>& types,
    std::vector<lex::Token> commas) -> NodePtr;

[[nodiscard]] auto errInvalidStmtEnumDecl(
    lex::Token kw,
    lex::Token id,
    lex::Token eq,
    const std::vector<EnumDeclStmtNode::EntrySpec>& entries,
    std::vector<lex::Token> commas) -> NodePtr;

[[nodiscard]] auto errInvalidPrimaryExpr(lex::Token token) -> NodePtr;

[[nodiscard]] auto errInvalidUnaryOp(lex::Token op, NodePtr rhs) -> NodePtr;

[[nodiscard]] auto errInvalidParenExpr(lex::Token open, NodePtr expr, lex::Token close) -> NodePtr;

[[nodiscard]] auto errInvalidFieldExpr(NodePtr lhs, lex::Token dot, lex::Token id) -> NodePtr;

[[nodiscard]] auto errInvalidIdExpr(lex::Token id, std::optional<TypeParamList> typeParams)
    -> NodePtr;

[[nodiscard]] auto
errInvalidIsExpr(NodePtr lhs, lex::Token kw, const Type& type, std::optional<lex::Token> id)
    -> NodePtr;

[[nodiscard]] auto errInvalidCallExpr(
    std::vector<lex::Token> modifiers,
    NodePtr lhs,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) -> NodePtr;

[[nodiscard]] auto errInvalidIndexExpr(
    NodePtr lhs,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) -> NodePtr;

[[nodiscard]] auto errInvalidConditionalExpr(
    NodePtr cond, lex::Token qmark, NodePtr ifBranch, lex::Token colon, NodePtr elseBranch)
    -> NodePtr;

[[nodiscard]] auto errInvalidSwitchIf(lex::Token kw, NodePtr cond, lex::Token arrow, NodePtr rhs)
    -> NodePtr;

[[nodiscard]] auto errInvalidSwitchElse(lex::Token kw, lex::Token arrow, NodePtr rhs) -> NodePtr;

} // namespace parse
