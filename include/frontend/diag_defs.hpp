#pragma once
#include "frontend/diag.hpp"
#include "frontend/source.hpp"
#include "parse/node_error.hpp"

namespace frontend {

auto errParseError(const Source& src, const parse::ErrorNode& n) -> Diag;

auto errUndeclaredType(const Source& src, const std::string& typeName, input::Span span) -> Diag;

auto errFuncNameConflictsWithType(const Source& src, const std::string& funcName, input::Span span)
    -> Diag;

auto errFuncNameConflictsWithAction(
    const Source& src, const std::string& funcName, input::Span span) -> Diag;

auto errDuplicateFuncDeclaration(const Source& src, const std::string& funcName, input::Span span)
    -> Diag;

} // namespace frontend
