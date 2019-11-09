#pragma once
#include "frontend/diag.hpp"
#include "frontend/source.hpp"
#include "parse/node_error.hpp"

namespace frontend {

auto errParseError(const Source& src, const parse::ErrorNode& n) -> Diag;

auto errUnsupportedLiteral(const Source& src, const std::string& name, input::Span span) -> Diag;

auto errFuncNameConflictsWithType(const Source& src, const std::string& name, input::Span span)
    -> Diag;

auto errFuncNameConflictsWithAction(const Source& src, const std::string& name, input::Span span)
    -> Diag;

auto errDuplicateFuncDeclaration(const Source& src, const std::string& name, input::Span span)
    -> Diag;

auto errNonMatchingFuncReturnType(
    const Source& src,
    const std::string& name,
    const std::string& declaredType,
    const std::string& returnedType,
    input::Span span) -> Diag;

auto errConstNameConflictsWithType(const Source& src, const std::string& name, input::Span span)
    -> Diag;

auto errConstNameConflictsWithFunction(const Source& src, const std::string& name, input::Span span)
    -> Diag;

auto errConstNameConflictsWithAction(const Source& src, const std::string& name, input::Span span)
    -> Diag;

auto errConstNameConflictsWithConst(const Source& src, const std::string& name, input::Span span)
    -> Diag;

auto errUndeclaredType(const Source& src, const std::string& name, input::Span span) -> Diag;

auto errUndeclaredConst(const Source& src, const std::string& name, input::Span span) -> Diag;

auto errUninitializedConst(const Source& src, const std::string& name, input::Span span) -> Diag;

auto errUndeclaredFunc(const Source& src, const std::string& name, input::Span span) -> Diag;

auto errUndeclaredFuncOverload(
    const Source& src,
    const std::string& name,
    const std::vector<const std::string*>& argTypes,
    input::Span span) -> Diag;

auto errUndeclaredAction(const Source& src, const std::string& name, input::Span span) -> Diag;

auto errUndeclaredActionOverload(
    const Source& src,
    const std::string& name,
    const std::vector<const std::string*>& argTypes,
    input::Span span) -> Diag;

auto errUnsupportedOperator(const Source& src, const std::string& name, input::Span span) -> Diag;

auto errUndeclaredUnaryOperator(
    const Source& src, const std::string& name, const std::string& type, input::Span span) -> Diag;

auto errUndeclaredBinOperator(
    const Source& src,
    const std::string& name,
    const std::string& lhsType,
    const std::string& rhsType,
    input::Span span) -> Diag;

auto errNonBoolExpressionInSwitch(const Source& src, const std::string& typeName, input::Span span)
    -> Diag;

auto errMismatchedBranchTypesInSwitch(
    const Source& src,
    const std::string& prevTypeName,
    const std::string& newTypeName,
    input::Span span) -> Diag;

} // namespace frontend
