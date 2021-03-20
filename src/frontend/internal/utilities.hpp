#pragma once
#include "internal/context.hpp"
#include "internal/typeinfer_expr.hpp"
#include "prog/operator.hpp"

namespace frontend::internal {

[[nodiscard]] auto getName(const lex::Token& token) -> std::string;

[[nodiscard]] auto getName(const parse::Type& parseType) -> std::string;

[[nodiscard]] auto getName(const Context& ctx, prog::sym::TypeId typeId) -> std::string;

[[nodiscard]] auto getDisplayName(const Context& ctx, prog::sym::TypeId typeId) -> std::string;

[[nodiscard]] auto getDisplayNames(const Context& ctx, prog::sym::TypeSet typeSet)
    -> std::vector<std::string>;

[[nodiscard]] auto getOperator(const lex::Token& token) -> std::optional<prog::Operator>;

[[nodiscard]] auto getText(const prog::Operator& op) -> std::string;

[[nodiscard]] auto isReservedTypeName(const std::string& name) -> bool;

[[nodiscard]] auto getOrInstType(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    const lex::Token& nameToken,
    const std::optional<parse::TypeParamList>& typeParams) -> std::optional<prog::sym::TypeId>;

[[nodiscard]] auto getOrInstType(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    const lex::Token& nameToken,
    const std::optional<parse::TypeParamList>& typeParams,
    const prog::sym::TypeSet& constructorArgs) -> std::optional<prog::sym::TypeId>;

[[nodiscard]] auto
getOrInstType(Context* ctx, const TypeSubstitutionTable* subTable, const parse::Type& parseType)
    -> std::optional<prog::sym::TypeId>;

[[nodiscard]] auto instType(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    const lex::Token& nameToken,
    const parse::TypeParamList& typeParams) -> std::optional<prog::sym::TypeId>;

[[nodiscard]] auto getRetType(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    const std::optional<parse::RetTypeSpec>& spec) -> std::optional<prog::sym::TypeId>;

template <typename FuncParseNode>
[[nodiscard]] auto inferRetType(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    const FuncParseNode& parseNode,
    const prog::sym::TypeSet& input,
    const std::unordered_map<std::string, prog::sym::TypeId>* additionalConstTypes,
    TypeInferExpr::Flags flags) -> prog::sym::TypeId;

[[nodiscard]] auto getDelegate(Context* ctx, prog::sym::FuncId func) -> prog::sym::TypeId;

[[nodiscard]] auto getLitFunc(Context* ctx, prog::sym::FuncId func) -> prog::expr::NodePtr;

[[nodiscard]] auto
getFuncClosure(Context* ctx, prog::sym::FuncId func, std::vector<prog::expr::NodePtr> boundArgs)
    -> prog::expr::NodePtr;

template <typename FuncParseNode>
[[nodiscard]] auto
getFuncInput(Context* ctx, const TypeSubstitutionTable* subTable, const FuncParseNode& parseNode)
    -> std::optional<prog::sym::TypeSet>;

template <typename FuncParseNode>
[[nodiscard]] auto getNumOptInputs(Context* ctx, const FuncParseNode& parseNode) -> unsigned int;

template <typename FuncParseNode>
[[nodiscard]] auto declareFuncInput(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    const FuncParseNode& n,
    prog::sym::ConstDeclTable* consts,
    bool allowArgInitializer) -> bool;

[[nodiscard]] auto getSubstitutionParams(Context* ctx, const parse::TypeSubstitutionList& subList)
    -> std::optional<std::vector<std::string>>;

[[nodiscard]] auto getTypeSet(
    Context* ctx, const TypeSubstitutionTable* subTable, const std::vector<parse::Type>& parseTypes)
    -> std::optional<prog::sym::TypeSet>;

[[nodiscard]] auto getTypeSet(const std::vector<prog::expr::NodePtr>& exprs) -> prog::sym::TypeSet;

[[nodiscard]] auto getConstName(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    const prog::sym::ConstDeclTable& consts,
    const lex::Token& nameToken) -> std::optional<std::string>;

[[nodiscard]] auto
mangleName(Context* ctx, const std::string& name, const prog::sym::TypeSet& typeParams)
    -> std::string;

[[nodiscard]] auto asFuture(Context* ctx, prog::sym::TypeId type) -> prog::sym::TypeId;

[[nodiscard]] auto funcOutAsFuture(Context* ctx, prog::sym::FuncId func) -> prog::sym::TypeId;

[[nodiscard]] auto delegateOutAsFuture(Context* ctx, prog::sym::TypeId delegate)
    -> std::optional<prog::sym::TypeId>;

[[nodiscard]] auto asLazy(Context* ctx, prog::sym::TypeId type, bool isAction) -> prog::sym::TypeId;

[[nodiscard]] auto funcOutAsLazy(Context* ctx, prog::sym::FuncId func) -> prog::sym::TypeId;

[[nodiscard]] auto delegateOutAsLazy(Context* ctx, prog::sym::TypeId delegate)
    -> std::optional<prog::sym::TypeId>;

[[nodiscard]] auto
isType(Context* ctx, const TypeSubstitutionTable* subTable, const std::string& name) -> bool;

[[nodiscard]] auto
isFuncOrConv(Context* ctx, const TypeSubstitutionTable* subTable, const std::string& name) -> bool;

[[nodiscard]] auto isPure(const Context* ctx, prog::sym::FuncId func) -> bool;

[[nodiscard]] auto isAction(const Context* ctx, prog::sym::FuncId func) -> bool;

} // namespace frontend::internal
