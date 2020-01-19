#pragma once
#include "internal/context.hpp"
#include "internal/typeinfer_expr.hpp"
#include "lex/token_payload_id.hpp"
#include "prog/operator.hpp"

namespace frontend::internal {

[[nodiscard]] auto getName(const lex::Token& token) -> std::string;

[[nodiscard]] auto getName(const parse::Type& parseType) -> std::string;

[[nodiscard]] auto getName(const Context& context, prog::sym::TypeId typeId) -> std::string;

[[nodiscard]] auto getDisplayName(const Context& context, prog::sym::TypeId typeId) -> std::string;

[[nodiscard]] auto getOperator(const lex::Token& token) -> std::optional<prog::Operator>;

[[nodiscard]] auto getText(const prog::Operator& op) -> std::string;

[[nodiscard]] auto isReservedTypeName(const std::string& name) -> bool;

[[nodiscard]] auto getOrInstType(
    Context* context,
    const TypeSubstitutionTable* subTable,
    const lex::Token& nameToken,
    const std::optional<parse::TypeParamList>& typeParams) -> std::optional<prog::sym::TypeId>;

[[nodiscard]] auto getOrInstType(
    Context* context,
    const TypeSubstitutionTable* subTable,
    const lex::Token& nameToken,
    const std::optional<parse::TypeParamList>& typeParams,
    const prog::sym::TypeSet& constructorArgs) -> std::optional<prog::sym::TypeId>;

[[nodiscard]] auto
getOrInstType(Context* context, const TypeSubstitutionTable* subTable, const parse::Type& parseType)
    -> std::optional<prog::sym::TypeId>;

[[nodiscard]] auto instType(
    Context* context,
    const TypeSubstitutionTable* subTable,
    const lex::Token& nameToken,
    const parse::TypeParamList& typeParams) -> std::optional<prog::sym::TypeId>;

[[nodiscard]] auto getRetType(
    Context* context, const TypeSubstitutionTable* subTable, const parse::FuncDeclStmtNode& n)
    -> std::optional<prog::sym::TypeId>;

template <typename FuncParseNode>
[[nodiscard]] auto inferRetType(
    Context* context,
    const TypeSubstitutionTable* subTable,
    const FuncParseNode& parseNode,
    const prog::sym::TypeSet& input,
    const std::unordered_map<std::string, prog::sym::TypeId>* additionalConstTypes,
    TypeInferExpr::Flags flags) -> prog::sym::TypeId;

[[nodiscard]] auto getLitFunc(Context* context, prog::sym::FuncId func) -> prog::expr::NodePtr;

[[nodiscard]] auto
getFuncClosure(Context* context, prog::sym::FuncId func, std::vector<prog::expr::NodePtr> boundArgs)
    -> prog::expr::NodePtr;

template <typename FuncParseNode>
[[nodiscard]] auto getFuncInput(
    Context* context, const TypeSubstitutionTable* subTable, const FuncParseNode& parseNode)
    -> std::optional<prog::sym::TypeSet>;

template <typename FuncParseNode>
[[nodiscard]] auto declareFuncInput(
    Context* context,
    const TypeSubstitutionTable* subTable,
    const FuncParseNode& n,
    prog::sym::ConstDeclTable* consts) -> bool;

[[nodiscard]] auto
getSubstitutionParams(Context* context, const parse::TypeSubstitutionList& subList)
    -> std::optional<std::vector<std::string>>;

[[nodiscard]] auto getTypeSet(
    Context* context,
    const TypeSubstitutionTable* subTable,
    const std::vector<parse::Type>& parseTypes) -> std::optional<prog::sym::TypeSet>;

[[nodiscard]] auto getConstName(
    Context* context,
    const TypeSubstitutionTable* subTable,
    const prog::sym::ConstDeclTable& consts,
    const lex::Token& nameToken) -> std::optional<std::string>;

[[nodiscard]] auto
mangleName(Context* context, const std::string& name, const prog::sym::TypeSet& typeParams)
    -> std::string;

[[nodiscard]] auto isType(Context* context, const std::string& name) -> bool;

[[nodiscard]] auto isFuncOrConv(Context* context, const std::string& name) -> bool;

[[nodiscard]] auto isPure(const Context* context, prog::sym::FuncId func) -> bool;

[[nodiscard]] auto isAction(const Context* context, prog::sym::FuncId func) -> bool;

} // namespace frontend::internal
