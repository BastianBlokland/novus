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

auto errTypeAlreadyDeclared(const Source& src, const std::string& name, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "Type name '" << name << "' conflicts with an previously declared type";
  return error(src, oss.str(), span);
}

auto errTypeTemplateAlreadyDeclared(const Source& src, const std::string& name, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "Type name '" << name << "' conflicts with an previously declared type template";
  return error(src, oss.str(), span);
}

auto errTypeNameConflictsWithFunc(const Source& src, const std::string& name, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "Type name '" << name << "' conflicts with a build-in function with the same name";
  return error(src, oss.str(), span);
}

auto errTypeNameConflictsWithAction(const Source& src, const std::string& name, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "Type name '" << name << "' conflicts with a build-in action with the same name";
  return error(src, oss.str(), span);
}

auto errDuplicateFieldNameInStruct(
    const Source& src, const std::string& fieldName, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "Field name '" << fieldName << "' conflicts with another field with the same name";
  return error(src, oss.str(), span);
}

auto errFieldNameConflictsWithTypeSubstitution(
    const Source& src, const std::string& fieldName, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "Field name '" << fieldName << "' conflicts with a type-substitution with the same name";
  return error(src, oss.str(), span);
}

auto errCyclicStruct(
    const Source& src,
    const std::string& fieldName,
    const std::string& structName,
    input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "Field '" << fieldName << "' causes struct '" << structName << "' to become cyclic";
  return error(src, oss.str(), span);
}

auto errFieldNameConflictsWithType(const Source& src, const std::string& name, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "Field name '" << name << "' conflicts with a type with the same name";
  return error(src, oss.str(), span);
}

auto errFieldNotFoundOnType(
    const Source& src, const std::string& fieldName, const std::string& typeName, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "Type '" << typeName << "' has no field named '" << fieldName << '\'';
  return error(src, oss.str(), span);
}

auto errDuplicateTypeInUnion(const Source& src, const std::string& typeName, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "Type '" << typeName << "' is already part of the same union";
  return error(src, oss.str(), span);
}

auto errNonUnionIsExpression(const Source& src, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "Left-hand-side of 'is' expression has to be a union type.";
  return error(src, oss.str(), span);
}

auto errTypeNotPartOfUnion(
    const Source& src, const std::string& typeName, const std::string& unionName, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "Type '" << typeName << "' is not part of the union '" << unionName << "'.";
  return error(src, oss.str(), span);
}

auto errUncheckedIsExpressionWithConst(const Source& src, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "Unchecked 'is' expression with constant declaration, either use in a checked context or "
         "discard '_' the const. ";
  return error(src, oss.str(), span);
}

auto errIncorrectReturnTypeInConvFunc(
    const Source& src, const std::string& name, const std::string& returnedType, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "Conversion function '" << name << "' returns incorrect type '" << returnedType << '\'';
  return error(src, oss.str(), span);
}

auto errConvFuncCallsItself(const Source& src, const std::string& name, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "Conversion function '" << name << "' cannot specify a return-type";
  return error(src, oss.str(), span);
}

auto errFuncNameConflictsWithAction(const Source& src, const std::string& name, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "Function name '" << name << "' conflicts with an action with the same name";
  return error(src, oss.str(), span);
}

auto errNonOverloadableOperator(const Source& src, const std::string& name, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "Operator '" << name << "' does not support overloading";
  return error(src, oss.str(), span);
}

auto errOperatorOverloadWithoutArgs(const Source& src, const std::string& name, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "Operator '" << name << "' cannot be defined without arguments";
  return error(src, oss.str(), span);
}

auto errTypeParamNameConflictsWithType(const Source& src, const std::string& name, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "Type parameter name '" << name << "' conflicts with a type with the same name";
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

auto errUnableToInferReturnTypeOfConversionToTemplatedType(
    const Source& src, const std::string& name, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "Unable to infer return-type of conversion '" << name
      << "' to templated type, please specify return-type using the '-> [TYPE]' syntax";
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
      << "' but its declared to return a value of type '" << declaredType << "'";
  return error(src, oss.str(), span);
}

auto errConstNameConflictsWithTypeSubstitution(
    const Source& src, const std::string& name, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "Constant name '" << name << "' conflicts with a type-substitution with the same name";
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

auto errUndeclaredTypeOrConversion(
    const Source& src,
    const std::string& name,
    const std::vector<std::string>& argTypes,
    input::Span span) -> Diag {
  std::ostringstream oss;
  if (argTypes.empty()) {
    oss << "No type or conversion '" << name << "' has been declared without any arguments";
  } else {
    oss << "No type or conversion '" << name << "' has been declared with argument types: ";
    for (auto i = 0U; i < argTypes.size(); ++i) {
      if (i != 0) {
        oss << ", ";
      }
      oss << '\'' << argTypes[i] << '\'';
    }
  }
  return error(src, oss.str(), span);
}

auto errNoTypeOrConversionFoundToInstantiate(
    const Source& src, const std::string& name, unsigned int templateParamCount, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "No templated type or conversion '" << name << "' has been declared with '"
      << templateParamCount << "' type parameters";
  return error(src, oss.str(), span);
}

auto errTypeParamOnSubstitutionType(const Source& src, const std::string& name, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "Type parameters cannot be applied to substitution type: '" << name << '\'';
  return error(src, oss.str(), span);
}

auto errInvalidTypeInstantiation(const Source& src, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "One or more errors occurred in type template instantiation";
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

auto errUndeclaredFunc(
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

auto errNoFuncFoundToInstantiate(
    const Source& src, const std::string& name, unsigned int templateParamCount, input::Span span)
    -> Diag {
  std::ostringstream oss;
  oss << "No templated function '" << name << "' has been declared with '" << templateParamCount
      << "' type parameters";
  return error(src, oss.str(), span);
}

auto errTypeParamsOnDynamicCallIsNotSupported(const Source& src, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "Dynamic calls do not support explicitly specified type arguments";
  return error(src, oss.str(), span);
}

auto errUndeclaredCallOperator(
    const Source& src, const std::vector<std::string>& argTypes, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "No overload for the call operator '()' has been declared with argument types: ";
  for (auto i = 0U; i < argTypes.size(); ++i) {
    if (i != 0) {
      oss << ", ";
    }
    oss << '\'' << argTypes[i] << '\'';
  }
  return error(src, oss.str(), span);
}

auto errUndeclaredIndexOperator(
    const Source& src, const std::vector<std::string>& argTypes, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "No overload for the index operator '[]' has been declared with argument types: ";
  for (auto i = 0U; i < argTypes.size(); ++i) {
    if (i != 0) {
      oss << ", ";
    }
    oss << '\'' << argTypes[i] << '\'';
  }
  return error(src, oss.str(), span);
}

auto errInvalidFuncInstantiation(const Source& src, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "One or more errors occurred in function template instantiation";
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

auto errBranchesHaveNoCommonType(const Source& src, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "Branches have no common type they are convertable to";
  return error(src, oss.str(), span);
}

auto errNoConversionFound(
    const Source& src, const std::string& from, const std::string& to, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "No conversion found from '" << from << "' to '" << to << '\'';
  return error(src, oss.str(), span);
}

auto errNonExhaustiveSwitchWithoutElse(const Source& src, input::Span span) -> Diag {
  std::ostringstream oss;
  oss << "Switch expression is missing an 'else' branch and cannot be guaranteed to be exhaustive";
  return error(src, oss.str(), span);
}

} // namespace frontend
