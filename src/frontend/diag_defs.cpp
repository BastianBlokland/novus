#include "frontend/diag_defs.hpp"
#include <sstream>

namespace frontend {

auto errUnresolvedImport(prog::sym::SourceId src, const std::string& path) -> Diag {
  std::ostringstream oss;
  oss << "Unable to resolve import '" << path << '\'';
  return error(oss.str(), src);
}

auto errParseError(prog::sym::SourceId src, const parse::ErrorNode& n) -> Diag {
  std::ostringstream oss;
  oss << n.getMessage();
  return error(oss.str(), src);
}

auto errUnsupportedLiteral(prog::sym::SourceId src, const std::string& name) -> Diag {
  std::ostringstream oss;
  oss << "Unsupported literal: " << name;
  return error(oss.str(), src);
}

auto errTypeAlreadyDeclared(prog::sym::SourceId src, const std::string& name) -> Diag {
  std::ostringstream oss;
  oss << "Type name '" << name << "' conflicts with an previously declared type";
  return error(oss.str(), src);
}

auto errTypeTemplateAlreadyDeclared(prog::sym::SourceId src, const std::string& name) -> Diag {
  std::ostringstream oss;
  oss << "Type name '" << name << "' conflicts with an previously declared type template";
  return error(oss.str(), src);
}

auto errTypeNameIsReserved(prog::sym::SourceId src, const std::string& name) -> Diag {
  std::ostringstream oss;
  oss << "Type name '" << name << "' is a reserved type-name that cannot be used for user types";
  return error(oss.str(), src);
}

auto errTypeNameConflictsWithFunc(prog::sym::SourceId src, const std::string& name) -> Diag {
  std::ostringstream oss;
  oss << "Type name '" << name << "' conflicts with a build-in function with the same name";
  return error(oss.str(), src);
}

auto errDuplicateFieldNameInStruct(prog::sym::SourceId src, const std::string& fieldName) -> Diag {
  std::ostringstream oss;
  oss << "Field name '" << fieldName << "' conflicts with another field with the same name";
  return error(oss.str(), src);
}

auto errFieldNameConflictsWithTypeSubstitution(
    prog::sym::SourceId src, const std::string& fieldName) -> Diag {
  std::ostringstream oss;
  oss << "Field name '" << fieldName << "' conflicts with a type-substitution with the same name";
  return error(oss.str(), src);
}

auto errCyclicStruct(
    prog::sym::SourceId src, const std::string& fieldName, const std::string& structName) -> Diag {
  std::ostringstream oss;
  oss << "Field '" << fieldName << "' causes struct '" << structName << "' to become cyclic";
  return error(oss.str(), src);
}

auto errFieldNameConflictsWithType(prog::sym::SourceId src, const std::string& name) -> Diag {
  std::ostringstream oss;
  oss << "Field name '" << name << "' conflicts with a type with the same name";
  return error(oss.str(), src);
}

auto errFieldNotFoundOnType(
    prog::sym::SourceId src, const std::string& fieldName, const std::string& typeName) -> Diag {
  std::ostringstream oss;
  oss << "Type '" << typeName << "' has no field named '" << fieldName << '\'';
  return error(oss.str(), src);
}

auto errStaticFieldNotFoundOnType(
    prog::sym::SourceId src, const std::string& fieldName, const std::string& typeName) -> Diag {
  std::ostringstream oss;
  oss << "Type '" << typeName << "' has no static field named '" << fieldName << '\'';
  return error(oss.str(), src);
}

auto errDuplicateTypeInUnion(
    prog::sym::SourceId src, const std::string& typeName, const std::string& substitutedTypeName)
    -> Diag {
  std::ostringstream oss;
  oss << "Type '" << typeName << '\'';
  if (typeName != substitutedTypeName) {
    oss << " ('" << substitutedTypeName << "')";
  }
  oss << " is already part of this union";
  return error(oss.str(), src);
}

auto errNonUnionIsExpression(prog::sym::SourceId src) -> Diag {
  std::ostringstream oss;
  oss << "Left-hand-side of 'is' expression has to be a union type";
  return error(oss.str(), src);
}

auto errTypeNotPartOfUnion(
    prog::sym::SourceId src, const std::string& typeName, const std::string& unionName) -> Diag {
  std::ostringstream oss;
  oss << "Type '" << typeName << "' is not part of the union '" << unionName << '\'';
  return error(oss.str(), src);
}

auto errUncheckedAsExpressionWithConst(prog::sym::SourceId src) -> Diag {
  std::ostringstream oss;
  oss << "Unchecked 'as' expression with constant declaration, either use in a checked context or "
         "discard '_' the const";
  return error(oss.str(), src);
}

auto errDuplicateEntryNameInEnum(prog::sym::SourceId src, const std::string& entryName) -> Diag {
  std::ostringstream oss;
  oss << "Name '" << entryName << "' conflicts with a previous entry in the same enum";
  return error(oss.str(), src);
}

auto errDuplicateEntryValueInEnum(prog::sym::SourceId src, int32_t entryValue) -> Diag {
  std::ostringstream oss;
  oss << "Value '" << entryValue << "' conflicts with a previous entry in the same enum";
  return error(oss.str(), src);
}

auto errValueNotFoundInEnum(
    prog::sym::SourceId src, const std::string& entryName, const std::string& enumName) -> Diag {
  std::ostringstream oss;
  oss << "Enum '" << enumName << "' does not contain '" << entryName << '\'';
  return error(oss.str(), src);
}

auto errIncorrectReturnTypeInConvFunc(
    prog::sym::SourceId src, const std::string& name, const std::string& returnedType) -> Diag {
  std::ostringstream oss;
  oss << "Conversion function '" << name << "' returns incorrect type '" << returnedType << '\'';
  return error(oss.str(), src);
}

auto errNonOverloadableOperator(prog::sym::SourceId src, const std::string& name) -> Diag {
  std::ostringstream oss;
  oss << "Operator '" << name << "' does not support overloading";
  return error(oss.str(), src);
}

auto errNonPureOperatorOverload(prog::sym::SourceId src) -> Diag {
  std::ostringstream oss;
  oss << "Operator overloads have to be pure ('fun' instead of 'act')";
  return error(oss.str(), src);
}

auto errOperatorOverloadWithoutArgs(prog::sym::SourceId src, const std::string& name) -> Diag {
  std::ostringstream oss;
  oss << "Operator '" << name << "' cannot be defined without arguments";
  return error(oss.str(), src);
}

auto errTypeParamNameConflictsWithType(prog::sym::SourceId src, const std::string& name) -> Diag {
  std::ostringstream oss;
  oss << "Type parameter name '" << name << "' conflicts with a type with the same name";
  return error(oss.str(), src);
}

auto errDuplicateFuncDeclaration(prog::sym::SourceId src, const std::string& name) -> Diag {
  std::ostringstream oss;
  oss << "Declaration of function '" << name
      << "' conflicts with an existing function with the same name and inputs";
  return error(oss.str(), src);
}

auto errUnableToInferFuncReturnType(prog::sym::SourceId src, const std::string& name) -> Diag {
  std::ostringstream oss;
  oss << "Unable to infer return-type of function '" << name
      << "', please specify return-type using the '-> [TYPE]' syntax";
  return error(oss.str(), src);
}

auto errUnableToInferReturnTypeOfConversionToTemplatedType(
    prog::sym::SourceId src, const std::string& name) -> Diag {
  std::ostringstream oss;
  oss << "Unable to infer return-type of conversion '" << name
      << "' to templated type, please specify return-type using the '-> [TYPE]' syntax";
  return error(oss.str(), src);
}

auto errNonMatchingFuncReturnType(
    prog::sym::SourceId src,
    const std::string& name,
    const std::string& declaredType,
    const std::string& returnedType) -> Diag {
  std::ostringstream oss;
  oss << "Function '" << name << "' returns value of type '" << returnedType
      << "' but its declared to return a value of type '" << declaredType << "'";
  return error(oss.str(), src);
}

auto errNonMatchingInitializerType(
    prog::sym::SourceId src, const std::string& declaredType, const std::string& initializerType)
    -> Diag {
  std::ostringstream oss;
  oss << "Initializer returns value of type '" << initializerType
      << "' which is incompatible with the declared type '" << declaredType << "'";
  return error(oss.str(), src);
}

auto errConstNameConflictsWithTypeSubstitution(prog::sym::SourceId src, const std::string& name)
    -> Diag {
  std::ostringstream oss;
  oss << "Constant name '" << name << "' conflicts with a type-substitution with the same name";
  return error(oss.str(), src);
}

auto errUnableToInferLambdaReturnType(prog::sym::SourceId src) -> Diag {
  std::ostringstream oss;
  oss << "Unable to infer return-type of lambda, please specify return-type using the '-> [TYPE]' "
         "syntax";
  return error(oss.str(), src);
}

auto errConstNameConflictsWithType(prog::sym::SourceId src, const std::string& name) -> Diag {
  std::ostringstream oss;
  oss << "Constant name '" << name << "' conflicts with a type with the same name";
  return error(oss.str(), src);
}

auto errConstNameConflictsWithConst(prog::sym::SourceId src, const std::string& name) -> Diag {
  std::ostringstream oss;
  oss << "Constant name '" << name
      << "' conflicts with an already declared constant in the same scope";
  return error(oss.str(), src);
}

auto errConstDeclareNotSupported(prog::sym::SourceId src) -> Diag {
  std::ostringstream oss;
  oss << "Declaring constants is not supported in this context";
  return error(oss.str(), src);
}

auto errUndeclaredType(prog::sym::SourceId src, const std::string& name, unsigned int typeParams)
    -> Diag {
  std::ostringstream oss;
  oss << "Unknown type: '" << name << '\'';
  if (typeParams == 0) {
    oss << " with no type parameters";
  } else {
    oss << " with '" << typeParams << "' type parameters";
  }
  return error(oss.str(), src);
}

auto errUndeclaredTypeOrConversion(
    prog::sym::SourceId src, const std::string& name, const std::vector<std::string>& argTypes)
    -> Diag {
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
  return error(oss.str(), src);
}

auto errNoTypeOrConversionFoundToInstantiate(
    prog::sym::SourceId src, const std::string& name, unsigned int templateParamCount) -> Diag {
  std::ostringstream oss;
  oss << "No templated type or conversion '" << name << "' has been declared with '"
      << templateParamCount << "' type parameters";
  return error(oss.str(), src);
}

auto errTypeParamOnSubstitutionType(prog::sym::SourceId src, const std::string& name) -> Diag {
  std::ostringstream oss;
  oss << "Type parameters cannot be applied to substitution type: '" << name << '\'';
  return error(oss.str(), src);
}

auto errInvalidTypeInstantiation(prog::sym::SourceId src) -> Diag {
  std::ostringstream oss;
  oss << "One or more errors occurred in type template instantiation";
  return error(oss.str(), src);
}

auto errUndeclaredConst(prog::sym::SourceId src, const std::string& name) -> Diag {
  std::ostringstream oss;
  oss << "No constant named '" << name << "' has been declared in the current scope";
  return error(oss.str(), src);
}

auto errUninitializedConst(prog::sym::SourceId src, const std::string& name) -> Diag {
  std::ostringstream oss;
  oss << "Constant '" << name << "' is not initialized at point of access";
  return error(oss.str(), src);
}

auto errUndeclaredPureFunc(
    prog::sym::SourceId src, const std::string& name, const std::vector<std::string>& argTypes)
    -> Diag {
  std::ostringstream oss;
  if (argTypes.empty()) {
    oss << "No overload for a pure function called: '" << name
        << "' has been declared without any arguments";
  } else {
    oss << "No overload for a pure function called: '" << name
        << "' has been declared with argument types: ";
    for (auto i = 0U; i < argTypes.size(); ++i) {
      if (i != 0) {
        oss << ", ";
      }
      oss << '\'' << argTypes[i] << '\'';
    }
  }
  return error(oss.str(), src);
}

auto errUndeclaredAction(
    prog::sym::SourceId src, const std::string& name, const std::vector<std::string>& argTypes)
    -> Diag {
  std::ostringstream oss;
  if (argTypes.empty()) {
    oss << "No overload for an action named '" << name
        << "' has been declared without any arguments";
  } else {
    oss << "No overload for an action named '" << name
        << "' has been declared with argument types: ";
    for (auto i = 0U; i < argTypes.size(); ++i) {
      if (i != 0) {
        oss << ", ";
      }
      oss << '\'' << argTypes[i] << '\'';
    }
  }
  return error(oss.str(), src);
}

auto errUndeclaredFuncOrAction(
    prog::sym::SourceId src, const std::string& name, const std::vector<std::string>& argTypes)
    -> Diag {
  std::ostringstream oss;
  if (argTypes.empty()) {
    oss << "No overload for a function or action called: '" << name
        << "' has been declared without any arguments";
  } else {
    oss << "No overload for a function or action called: '" << name
        << "' has been declared with argument types: ";
    for (auto i = 0U; i < argTypes.size(); ++i) {
      if (i != 0) {
        oss << ", ";
      }
      oss << '\'' << argTypes[i] << '\'';
    }
  }
  return error(oss.str(), src);
}

auto errUnknownIntrinsic(
    prog::sym::SourceId src,
    const std::string& name,
    bool pureOnly,
    const std::vector<std::string>& argTypes) -> Diag {
  std::ostringstream oss;
  oss << "No " << (pureOnly ? "pure " : "") << "compiler intrinsic called: '" << name << "' ";
  if (argTypes.empty()) {
    oss << "found without arguments";
  } else {
    oss << "found with argument types: ";
    for (auto i = 0U; i < argTypes.size(); ++i) {
      if (i != 0) {
        oss << ", ";
      }
      oss << '\'' << argTypes[i] << '\'';
    }
  }
  return error(oss.str(), src);
}

auto errPureFuncInfRecursion(prog::sym::SourceId src) -> Diag {
  std::ostringstream oss;
  oss << "Pure function recurses infinitely";
  return error(oss.str(), src);
}

auto errNoPureFuncFoundToInstantiate(
    prog::sym::SourceId src, const std::string& name, unsigned int templateParamCount) -> Diag {
  std::ostringstream oss;
  oss << "No templated pure function '" << name << "' has been declared with '"
      << templateParamCount << "' type parameters";
  return error(oss.str(), src);
}

auto errNoActionFoundToInstantiate(
    prog::sym::SourceId src, const std::string& name, unsigned int templateParamCount) -> Diag {
  std::ostringstream oss;
  oss << "No templated action '" << name << "' has been declared with '" << templateParamCount
      << "' type parameters";
  return error(oss.str(), src);
}

auto errNoFuncOrActionFoundToInstantiate(
    prog::sym::SourceId src, const std::string& name, unsigned int templateParamCount) -> Diag {
  std::ostringstream oss;
  oss << "No templated function or action '" << name << "' has been declared with '"
      << templateParamCount << "' type parameters";
  return error(oss.str(), src);
}

auto errNoTypeParamsProvidedToTemplateFunction(prog::sym::SourceId src, const std::string& name)
    -> Diag {
  std::ostringstream oss;
  oss << "No type parameters provided to function template '" << name << '\'';
  return error(oss.str(), src);
}

auto errAmbiguousFunction(prog::sym::SourceId src, const std::string& name) -> Diag {
  std::ostringstream oss;
  oss << "Ambiguous function, multiple functions named '" << name << '\'';
  return error(oss.str(), src);
}

auto errAmbiguousTemplateFunction(
    prog::sym::SourceId src, const std::string& name, unsigned int templateParamCount) -> Diag {
  std::ostringstream oss;
  oss << "Ambiguous function, multiple templated functions named '" << name << "' with '"
      << templateParamCount << "' type parameters";
  return error(oss.str(), src);
}

auto errIllegalDelegateCall(prog::sym::SourceId src) -> Diag {
  std::ostringstream oss;
  oss << "Cannot invoke the delegate, action's cannot be called from pure functions";
  return error(oss.str(), src);
}

auto errIncorrectArgsToDelegate(prog::sym::SourceId src) -> Diag {
  std::ostringstream oss;
  oss << "Incorrect arguments provided to delegate";
  return error(oss.str(), src);
}

auto errUndeclaredCallOperator(prog::sym::SourceId src, const std::vector<std::string>& argTypes)
    -> Diag {
  std::ostringstream oss;
  oss << "No overload for the call operator '()' has been declared with argument types: ";
  for (auto i = 0U; i < argTypes.size(); ++i) {
    if (i != 0) {
      oss << ", ";
    }
    oss << '\'' << argTypes[i] << '\'';
  }
  return error(oss.str(), src);
}

auto errUndeclaredIndexOperator(prog::sym::SourceId src, const std::vector<std::string>& argTypes)
    -> Diag {
  std::ostringstream oss;
  oss << "No overload for the index operator '[]' has been declared with argument types: ";
  for (auto i = 0U; i < argTypes.size(); ++i) {
    if (i != 0) {
      oss << ", ";
    }
    oss << '\'' << argTypes[i] << '\'';
  }
  return error(oss.str(), src);
}

auto errInvalidFuncInstantiation(prog::sym::SourceId src) -> Diag {
  std::ostringstream oss;
  oss << "One or more errors occurred in function template instantiation";
  return error(oss.str(), src);
}

auto errUnsupportedOperator(prog::sym::SourceId src, const std::string& name) -> Diag {
  std::ostringstream oss;
  oss << "Unsupported operator '" << name << '\'';
  return error(oss.str(), src);
}

auto errUndeclaredUnaryOperator(
    prog::sym::SourceId src, const std::string& name, const std::string& type) -> Diag {
  std::ostringstream oss;
  oss << "No overload for unary operator '" << name << "' has been declared for type: '" << type
      << '\'';
  return error(oss.str(), src);
}

auto errUndeclaredBinOperator(
    prog::sym::SourceId src,
    const std::string& name,
    const std::string& lhsType,
    const std::string& rhsType) -> Diag {
  std::ostringstream oss;
  oss << "No overload for binary operator '" << name << "' has been declared for types: '"
      << lhsType << "' and '" << rhsType << '\'';
  return error(oss.str(), src);
}

auto errBranchesHaveNoCommonType(prog::sym::SourceId src) -> Diag {
  std::ostringstream oss;
  oss << "Branches have no common type they are convertable to";
  return error(oss.str(), src);
}

auto errNoImplicitConversionFound(
    prog::sym::SourceId src, const std::string& from, const std::string& to) -> Diag {
  std::ostringstream oss;
  oss << "No implicit conversion found from '" << from << "' to '" << to << '\'';
  return error(oss.str(), src);
}

auto errNonExhaustiveSwitchWithoutElse(prog::sym::SourceId src) -> Diag {
  std::ostringstream oss;
  oss << "Switch expression is missing an 'else' branch and cannot be guaranteed to be exhaustive";
  return error(oss.str(), src);
}

auto errNonPureConversion(prog::sym::SourceId src) -> Diag {
  std::ostringstream oss;
  oss << "Type conversion has to be pure ('fun' instead of 'act')";
  return error(oss.str(), src);
}

auto errForkedNonUserFunc(prog::sym::SourceId src) -> Diag {
  std::ostringstream oss;
  oss << "Only user-defined functions can be forked";
  return error(oss.str(), src);
}

auto errLazyNonUserFunc(prog::sym::SourceId src) -> Diag {
  std::ostringstream oss;
  oss << "Only user-defined functions can be called lazily";
  return error(oss.str(), src);
}

auto errForkedSelfCall(prog::sym::SourceId src) -> Diag {
  std::ostringstream oss;
  oss << "Self calls cannot be forked";
  return error(oss.str(), src);
}

auto errLazySelfCall(prog::sym::SourceId src) -> Diag {
  std::ostringstream oss;
  oss << "Self calls cannot be lazy";
  return error(oss.str(), src);
}

auto errSelfCallInNonFunc(prog::sym::SourceId src) -> Diag {
  std::ostringstream oss;
  oss << "Self calls can only be used inside functions or actions";
  return error(oss.str(), src);
}

auto errSelfCallWithoutInferredRetType(prog::sym::SourceId src) -> Diag {
  std::ostringstream oss;
  oss << "Failed to bind self call as return type could not be inferred";
  return error(oss.str(), src);
}

auto errIncorrectNumArgsInSelfCall(
    prog::sym::SourceId src, unsigned int expectedNumArgs, unsigned int actualNumArgs) -> Diag {
  std::ostringstream oss;
  oss << "Self call requires '" << expectedNumArgs << "' arguments but got '" << actualNumArgs
      << "'";
  return error(oss.str(), src);
}

auto errInvalidFailIntrinsicCall(
    prog::sym::SourceId src, unsigned int typeParams, unsigned int argCount) -> Diag {
  std::ostringstream oss;
  oss << "Invalid fail intrinsic action call";
  if (typeParams != 1) {
    oss << ", requires '1' type parameter but got '" << typeParams << "'";
  }
  if (argCount != 0) {
    oss << ", requires '0' arguments but got '" << argCount << "'";
  }
  return error(oss.str(), src);
}

auto errIntrinsicFuncLiteral(prog::sym::SourceId src) -> Diag {
  std::ostringstream oss;
  oss << "Compiler intrinsic cannot be used as a function literal";
  return error(oss.str(), src);
}

auto errUnsupportedArgInitializer(prog::sym::SourceId src, const std::string& name) -> Diag {
  std::ostringstream oss;
  oss << "Initializer for input argument '" << name << "' is unsupported in the current context";
  return error(oss.str(), src);
}

auto errNonOptArgFollowingOpt(prog::sym::SourceId src) -> Diag {
  std::ostringstream oss;
  oss << "Required argument cannot follow an optional argument";
  return error(oss.str(), src);
}

auto errCyclicOptArgInitializer(prog::sym::SourceId src) -> Diag {
  std::ostringstream oss;
  oss << "Cycle detected while applying optional argument initializers";
  return error(oss.str(), src);
}

} // namespace frontend
