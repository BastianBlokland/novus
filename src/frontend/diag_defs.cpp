#include "frontend/diag_defs.hpp"
#include <sstream>

namespace frontend {

auto errParseError(const Source& src, const parse::ErrorNode& n) -> Diag {
  std::ostringstream oss;
  oss << "Parse error: " << n.getMessage();
  return error(src, oss.str(), n.getSpan());
}

auto errUnsupportedLiteral(const Source& src, const std::string& name, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "Unsupported literal: " << name;
  return error(src, oss.str(), span);
}

auto errFuncNameConflictsWithType(const Source& src, const std::string& name, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "Function name '" << name << "' conflicts with a type with the same name";
  return error(src, oss.str(), span);
}

auto errFuncNameConflictsWithAction(const Source& src, const std::string& name, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "Function name '" << name << "' conflicts with an action with the same name";
  return error(src, oss.str(), span);
}

auto errDuplicateFuncDeclaration(const Source& src, const std::string& name, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "Declaration of function '" << name
      << "' conflicts with an existing function with the same name and inputs";
  return error(src, oss.str(), span);
}

auto errUnableToInferFuncReturnType(const Source& src, const std::string& name, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "Unable to infer return-type of function '" << name
      << "', please specify return-type using the '-> [TYPE]' syntax";
  return error(src, oss.str(), span);
}

auto errNonMatchingFuncReturnType(
    const Source& src,
    const std::string& name,
    const std::string& declaredType,
    const std::string& returnedType,
    input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "Function '" << name << "' returns value of type '" << returnedType
      << "' but its declared to return values of type '" << declaredType << "'";
  return error(src, oss.str(), span);
}

auto errConstNameConflictsWithType(const Source& src, const std::string& name, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "Constant name '" << name << "' conflicts with a type with the same name";
  return error(src, oss.str(), span);
}

auto errConstNameConflictsWithFunction(const Source& src, const std::string& name, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "Constant name '" << name << "' conflicts with an function with the same name";
  return error(src, oss.str(), span);
}

auto errConstNameConflictsWithAction(const Source& src, const std::string& name, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "Constant name '" << name << "' conflicts with an action with the same name";
  return error(src, oss.str(), span);
}

auto errConstNameConflictsWithConst(const Source& src, const std::string& name, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "Constant name '" << name
      << "' conflicts with an already declared constant in the same scope";
  return error(src, oss.str(), span);
}

auto errUndeclaredType(const Source& src, const std::string& name, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "Unknown type: '" << name << '\'';
  return error(src, oss.str(), span);
}

auto errUndeclaredConst(const Source& src, const std::string& name, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "No constant named '" << name << "' has been declared in the current scope";
  return error(src, oss.str(), span);
}

auto errUninitializedConst(const Source& src, const std::string& name, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "Constant '" << name << "' is not initialized at point of access";
  return error(src, oss.str(), span);
}

auto errUndeclaredFunc(const Source& src, const std::string& name, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "No function named '" << name << "' has been declared";
  return error(src, oss.str(), span);
}

auto errUndeclaredFuncOverload(
    const Source& src,
    const std::string& name,
    const std::vector<std::string>& argTypes,
    input::Span span) -> Diag {
  std::ostringstream oss;
  if (argTypes.empty()) {
    oss << "No overload for function '" << name << "' has been declared without any arguments";
  } else {
    oss << "No overload for function '" << name << "' has been declared with argument types: ";
    for (auto i = 0U; i < argTypes.size(); ++i) {
      if (i != 0) {
        oss << ", ";
      }
      oss << '\'' << argTypes[i] << '\'';
    }
  }
  return error(src, oss.str(), span);
}

auto errUndeclaredAction(const Source& src, const std::string& name, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "No action named '" << name << "' has been declared";
  return error(src, oss.str(), span);
}

auto errUndeclaredActionOverload(
    const Source& src,
    const std::string& name,
    const std::vector<std::string>& argTypes,
    input::Span span) -> Diag {
  std::ostringstream oss;
  if (argTypes.empty()) {
    oss << "No overload for action '" << name << "' has been declared without any arguments";
  } else {
    oss << "No overload for action '" << name << "' has been declared with argument types: ";
    for (auto i = 0U; i < argTypes.size(); ++i) {
      if (i != 0) {
        oss << ", ";
      }
      oss << '\'' << argTypes[i] << '\'';
    }
  }
  return error(src, oss.str(), span);
}

auto errUnsupportedOperator(const Source& src, const std::string& name, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "Unsupported operator '" << name << '\'';
  return error(src, oss.str(), span);
}

auto errUndeclaredUnaryOperator(
    const Source& src, const std::string& name, const std::string& type, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "No overload for unary operator '" << name << "' has been declared for type: '" << type
      << '\'';
  return error(src, oss.str(), span);
}

auto errUndeclaredBinOperator(
    const Source& src,
    const std::string& name,
    const std::string& lhsType,
    const std::string& rhsType,
    input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "No overload for binary operator '" << name << "' has been declared for types: '"
      << lhsType << "' and '" << rhsType << '\'';
  return error(src, oss.str(), span);
}

auto errNonBoolExpressionInLogicOp(const Source& src, const std::string& typeName, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "Type in logical operation has to be 'bool' but was: '" << typeName << '\'';
  return error(src, oss.str(), span);
}

auto errNonBoolConditionExpression(const Source& src, const std::string& typeName, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "Type of conditional expression has to be 'bool' but was: '" << typeName << '\'';
  return error(src, oss.str(), span);
}

auto errMismatchedBranchTypes(
    const Source& src,
    const std::string& prevTypeName,
    const std::string& newTypeName,
    input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "Branch has type '" << newTypeName << "' but type of previous branch is '" << prevTypeName
      << '\'';
  return error(src, oss.str(), span);
}

} // namespace frontend
