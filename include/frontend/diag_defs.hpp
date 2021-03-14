#pragma once
#include "frontend/diag.hpp"
#include "parse/node_error.hpp"
#include "prog/sym/source_id.hpp"

namespace frontend {

[[nodiscard]] auto errUnresolvedImport(prog::sym::SourceId src, const std::string& path) -> Diag;

[[nodiscard]] auto errParseError(prog::sym::SourceId src, const parse::ErrorNode& n) -> Diag;

[[nodiscard]] auto errUnsupportedLiteral(prog::sym::SourceId src, const std::string& name) -> Diag;

[[nodiscard]] auto errTypeAlreadyDeclared(prog::sym::SourceId src, const std::string& name) -> Diag;

[[nodiscard]] auto errTypeTemplateAlreadyDeclared(prog::sym::SourceId src, const std::string& name)
    -> Diag;

[[nodiscard]] auto errTypeNameIsReserved(prog::sym::SourceId src, const std::string& name) -> Diag;

[[nodiscard]] auto errTypeNameConflictsWithFunc(prog::sym::SourceId src, const std::string& name)
    -> Diag;

[[nodiscard]] auto
errDuplicateFieldNameInStruct(prog::sym::SourceId src, const std::string& fieldName) -> Diag;

[[nodiscard]] auto
errFieldNameConflictsWithTypeSubstitution(prog::sym::SourceId src, const std::string& fieldName)
    -> Diag;

[[nodiscard]] auto errCyclicStruct(
    prog::sym::SourceId src, const std::string& fieldName, const std::string& structName) -> Diag;

[[nodiscard]] auto errFieldNameConflictsWithType(prog::sym::SourceId src, const std::string& name)
    -> Diag;

[[nodiscard]] auto errFieldNotFoundOnType(
    prog::sym::SourceId src, const std::string& fieldName, const std::string& typeName) -> Diag;

[[nodiscard]] auto errStaticFieldNotFoundOnType(
    prog::sym::SourceId src, const std::string& fieldName, const std::string& typeName) -> Diag;

[[nodiscard]] auto errDuplicateTypeInUnion(
    prog::sym::SourceId src, const std::string& typeName, const std::string& substitutedTypeName)
    -> Diag;

[[nodiscard]] auto errNonUnionIsExpression(prog::sym::SourceId src) -> Diag;

[[nodiscard]] auto errTypeNotPartOfUnion(
    prog::sym::SourceId src, const std::string& typeName, const std::string& unionName) -> Diag;

[[nodiscard]] auto errUncheckedAsExpressionWithConst(prog::sym::SourceId src) -> Diag;

[[nodiscard]] auto
errDuplicateEntryNameInEnum(prog::sym::SourceId src, const std::string& entryName) -> Diag;

[[nodiscard]] auto errDuplicateEntryValueInEnum(prog::sym::SourceId src, int32_t entryValue)
    -> Diag;

[[nodiscard]] auto errValueNotFoundInEnum(
    prog::sym::SourceId src, const std::string& entryName, const std::string& enumName) -> Diag;

[[nodiscard]] auto errIncorrectReturnTypeInConvFunc(
    prog::sym::SourceId src, const std::string& name, const std::string& returnedType) -> Diag;

[[nodiscard]] auto errNonOverloadableOperator(prog::sym::SourceId src, const std::string& name)
    -> Diag;

[[nodiscard]] auto errNonPureOperatorOverload(prog::sym::SourceId src) -> Diag;

[[nodiscard]] auto errOperatorOverloadWithoutArgs(prog::sym::SourceId src, const std::string& name)
    -> Diag;

[[nodiscard]] auto errTemplatedImplicitConversion(prog::sym::SourceId src) -> Diag;

[[nodiscard]] auto errImplicitNonConv(prog::sym::SourceId src) -> Diag;

[[nodiscard]] auto errToManyInputsInImplicitConv(prog::sym::SourceId src) -> Diag;

[[nodiscard]] auto
errTypeParamNameConflictsWithType(prog::sym::SourceId src, const std::string& name) -> Diag;

[[nodiscard]] auto errDuplicateTypeParamName(prog::sym::SourceId src, const std::string& name)
    -> Diag;

[[nodiscard]] auto errDuplicateFuncDeclaration(prog::sym::SourceId src, const std::string& name)
    -> Diag;

[[nodiscard]] auto errUnableToInferFuncReturnType(prog::sym::SourceId src, const std::string& name)
    -> Diag;

[[nodiscard]] auto errUnableToInferReturnTypeOfConversionToTemplatedType(
    prog::sym::SourceId src, const std::string& name) -> Diag;

[[nodiscard]] auto errNonMatchingFuncReturnType(
    prog::sym::SourceId src,
    const std::string& name,
    const std::string& declaredType,
    const std::string& returnedType) -> Diag;

[[nodiscard]] auto errNonMatchingInitializerType(
    prog::sym::SourceId src, const std::string& declaredType, const std::string& intializerType)
    -> Diag;

[[nodiscard]] auto errUnableToInferLambdaReturnType(prog::sym::SourceId src) -> Diag;

[[nodiscard]] auto errConstNameConflictsWithType(prog::sym::SourceId src, const std::string& name)
    -> Diag;

[[nodiscard]] auto
errConstNameConflictsWithTypeSubstitution(prog::sym::SourceId src, const std::string& name) -> Diag;

[[nodiscard]] auto errConstNameConflictsWithConst(prog::sym::SourceId src, const std::string& name)
    -> Diag;

[[nodiscard]] auto errConstDeclareNotSupported(prog::sym::SourceId src) -> Diag;

[[nodiscard]] auto
errUndeclaredType(prog::sym::SourceId src, const std::string& name, unsigned int typeParams)
    -> Diag;

[[nodiscard]] auto errUndeclaredTypeOrConversion(
    prog::sym::SourceId src, const std::string& name, const std::vector<std::string>& argTypes)
    -> Diag;

[[nodiscard]] auto errNoTypeOrConversionFoundToInstantiate(
    prog::sym::SourceId src, const std::string& name, unsigned int templateParamCount) -> Diag;

[[nodiscard]] auto errTypeParamOnSubstitutionType(prog::sym::SourceId src, const std::string& name)
    -> Diag;

[[nodiscard]] auto errInvalidTypeInstantiation(prog::sym::SourceId src) -> Diag;

[[nodiscard]] auto errUndeclaredConst(prog::sym::SourceId src, const std::string& name) -> Diag;

[[nodiscard]] auto errUninitializedConst(prog::sym::SourceId src, const std::string& name) -> Diag;

[[nodiscard]] auto errUndeclaredPureFunc(
    prog::sym::SourceId src, const std::string& name, const std::vector<std::string>& argTypes)
    -> Diag;

[[nodiscard]] auto errUndeclaredAction(
    prog::sym::SourceId src, const std::string& name, const std::vector<std::string>& argTypes)
    -> Diag;

[[nodiscard]] auto errUndeclaredFuncOrAction(
    prog::sym::SourceId src, const std::string& name, const std::vector<std::string>& argTypes)
    -> Diag;

[[nodiscard]] auto errUnknownIntrinsic(
    prog::sym::SourceId src,
    const std::string& name,
    bool pureOnly,
    const std::vector<std::string>& argTypes) -> Diag;

[[nodiscard]] auto errPureFuncInfRecursion(prog::sym::SourceId src) -> Diag;

[[nodiscard]] auto errNoPureFuncFoundToInstantiate(
    prog::sym::SourceId src, const std::string& name, unsigned int templateParamCount) -> Diag;

[[nodiscard]] auto errNoActionFoundToInstantiate(
    prog::sym::SourceId src, const std::string& name, unsigned int templateParamCount) -> Diag;

[[nodiscard]] auto errNoFuncOrActionFoundToInstantiate(
    prog::sym::SourceId src, const std::string& name, unsigned int templateParamCount) -> Diag;

[[nodiscard]] auto
errNoTypeParamsProvidedToTemplateFunction(prog::sym::SourceId src, const std::string& name) -> Diag;

[[nodiscard]] auto errAmbiguousFunction(prog::sym::SourceId src, const std::string& name) -> Diag;

[[nodiscard]] auto errAmbiguousTemplateFunction(
    prog::sym::SourceId src, const std::string& name, unsigned int templateParamCount) -> Diag;

[[nodiscard]] auto errIllegalDelegateCall(prog::sym::SourceId src) -> Diag;

[[nodiscard]] auto errIncorrectArgsToDelegate(prog::sym::SourceId src) -> Diag;

[[nodiscard]] auto
errUndeclaredCallOperator(prog::sym::SourceId src, const std::vector<std::string>& argTypes)
    -> Diag;

[[nodiscard]] auto
errUndeclaredIndexOperator(prog::sym::SourceId src, const std::vector<std::string>& argTypes)
    -> Diag;

[[nodiscard]] auto errInvalidFuncInstantiation(prog::sym::SourceId src) -> Diag;

[[nodiscard]] auto errUnsupportedOperator(prog::sym::SourceId src, const std::string& name) -> Diag;

[[nodiscard]] auto errUndeclaredUnaryOperator(
    prog::sym::SourceId src, const std::string& name, const std::string& type) -> Diag;

[[nodiscard]] auto errUndeclaredBinOperator(
    prog::sym::SourceId src,
    const std::string& name,
    const std::string& lhsType,
    const std::string& rhsType) -> Diag;

[[nodiscard]] auto errBranchesHaveNoCommonType(prog::sym::SourceId src) -> Diag;

[[nodiscard]] auto errNoImplicitConversionFound(
    prog::sym::SourceId src, const std::string& from, const std::string& to) -> Diag;

[[nodiscard]] auto errNonExhaustiveSwitchWithoutElse(prog::sym::SourceId src) -> Diag;

[[nodiscard]] auto errNonPureConversion(prog::sym::SourceId src) -> Diag;

[[nodiscard]] auto errForkedNonUserFunc(prog::sym::SourceId src) -> Diag;

[[nodiscard]] auto errLazyNonUserFunc(prog::sym::SourceId src) -> Diag;

[[nodiscard]] auto errForkedSelfCall(prog::sym::SourceId src) -> Diag;

[[nodiscard]] auto errLazySelfCall(prog::sym::SourceId src) -> Diag;

[[nodiscard]] auto errSelfCallInNonFunc(prog::sym::SourceId src) -> Diag;

[[nodiscard]] auto errSelfCallWithoutInferredRetType(prog::sym::SourceId src) -> Diag;

[[nodiscard]] auto errIncorrectNumArgsInSelfCall(
    prog::sym::SourceId src, unsigned int expectedNumArgs, unsigned int actualNumArgs) -> Diag;

[[nodiscard]] auto errIntrinsicFuncLiteral(prog::sym::SourceId src) -> Diag;

[[nodiscard]] auto errUnsupportedArgInitializer(prog::sym::SourceId src, const std::string& name)
    -> Diag;

[[nodiscard]] auto errNonOptArgFollowingOpt(prog::sym::SourceId src) -> Diag;

[[nodiscard]] auto errCyclicOptArgInitializer(prog::sym::SourceId src) -> Diag;

} // namespace frontend
