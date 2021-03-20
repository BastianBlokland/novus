#include "internal/typeinfer_expr.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/context.hpp"
#include "internal/get_identifier.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"
#include <cassert>
#include <optional>

namespace {

constexpr const size_t g_maxTypeInferRecurseDepth = 100;

// Keep track of the type-infer recursion depth, this is needed because with meta-programming you
// can actually build type or function definitions that recurse indefinitely.
// TODO: Implement this is a more elegant way.
thread_local size_t g_typeInferRecurseDepth = 0;

auto isMaxInferRecurseDepthReached() noexcept -> bool {
  return g_typeInferRecurseDepth >= g_maxTypeInferRecurseDepth;
}

} // namespace

namespace frontend::internal {

TypeInferExpr::TypeInferExpr(
    Context* ctx,
    const TypeSubstitutionTable* typeSubTable,
    std::unordered_map<std::string, prog::sym::TypeId>* constTypes,
    Flags flags) :
    m_ctx{ctx},
    m_typeSubTable{typeSubTable},
    m_constTypes{constTypes},
    m_flags{flags},
    m_type{prog::sym::TypeId::inferType()} {

  if (m_ctx == nullptr) {
    throw std::invalid_argument{"Context cannot be null"};
  }
  ++g_typeInferRecurseDepth;
}

TypeInferExpr::~TypeInferExpr() noexcept { --g_typeInferRecurseDepth; }

auto TypeInferExpr::getInferredType() const noexcept -> prog::sym::TypeId { return m_type; }

auto TypeInferExpr::visit(const parse::CommentNode& /*unused*/) -> void {
  throw std::logic_error{"TypeInferExpr is not implemented for this node type"};
}

auto TypeInferExpr::visit(const parse::ErrorNode& /*unused*/) -> void {
  throw std::logic_error{"TypeInferExpr is not implemented for this node type"};
}

auto TypeInferExpr::visit(const parse::AnonFuncExprNode& n) -> void {
  auto funcInput = getFuncInput(m_ctx, m_typeSubTable, n);
  if (!funcInput) {
    assert(m_ctx->hasErrors());
    return;
  }

  // Impure lambda's produce actions and are allowed to call other actions.
  auto isAction = n.isImpure();

  auto retType = getRetType(m_ctx, m_typeSubTable, n.getRetType());
  if (retType && retType->isInfer()) {
    retType = inferRetType(
        m_ctx,
        m_typeSubTable,
        n,
        *funcInput,
        m_constTypes,
        isAction ? (TypeInferExpr::Flags::AllowActionCalls | TypeInferExpr::Flags::Aggressive)
                 : TypeInferExpr::Flags::Aggressive);
  }

  if (retType && retType->isConcrete()) {
    m_type = m_ctx->getDelegates()->getDelegate(m_ctx, isAction, *funcInput, *retType);
  }
}

auto TypeInferExpr::visit(const parse::BinaryExprNode& n) -> void {
  const auto& opToken = n.getOperator();
  switch (opToken.getKind()) {
  case lex::TokenKind::OpAmpAmp:
  case lex::TokenKind::OpPipePipe: {
    m_type = m_ctx->getProg()->getBool();
    break;
  }
  default:
    break;
  }
  const auto op = getOperator(opToken);
  if (!op) {
    return;
  }
  auto argTypes = std::vector<prog::sym::TypeId>{};
  argTypes.reserve(n.getChildCount());
  for (auto i = 0U; i < n.getChildCount(); ++i) {
    argTypes.push_back(inferSubExpr(n[i]));
  }
  const auto argTypeSet = prog::sym::TypeSet{std::move(argTypes)};
  if (const auto callInfo = inferFuncCall(prog::getFuncName(*op), argTypeSet)) {
    m_type = callInfo->resultType;
  }
}

auto TypeInferExpr::visit(const parse::CallExprNode& n) -> void {
  if (isMaxInferRecurseDepthReached()) {
    return;
  }

  auto getIdVisitor = GetIdentifier{true};
  n[0].accept(&getIdVisitor);
  auto* instance  = getIdVisitor.getInstance();
  auto identifier = getIdVisitor.getIdentifier();
  auto typeParams = getIdVisitor.getTypeParams();

  if (getIdVisitor.isIntrinsic()) {
    return;
  }

  // Dynamic call.
  const auto isConst =
      m_constTypes && identifier && m_constTypes->find(getName(*identifier)) != m_constTypes->end();
  const auto isPotentialFieldCall =
      instance && !isFuncOrConv(m_ctx, m_typeSubTable, getName(*identifier));
  if (!identifier || isConst || isPotentialFieldCall) {
    m_type = inferDynCall(n);
    return;
  }
  auto nameToken = *identifier;
  auto funcName  = getName(nameToken);

  auto argTypes = std::vector<prog::sym::TypeId>{};
  argTypes.reserve(n.getChildCount());
  if (instance) {
    argTypes.push_back(inferSubExpr(*instance));
  }
  for (auto i = 1U; i < n.getChildCount(); ++i) {
    argTypes.push_back(inferSubExpr(n[i]));
  }
  const auto argTypeSet = prog::sym::TypeSet{std::move(argTypes)};

  // Check if this is calling a constructor / conversion.
  auto convType = getOrInstType(m_ctx, m_typeSubTable, nameToken, typeParams, argTypeSet);
  if (convType && convType->isConcrete()) {
    if (n.isFork()) {
      m_type = asFuture(m_ctx, *convType);
    } else if (n.isLazy()) {
      // Contructors and conversions are required to be pure so we know this will be a 'lazy' and
      // not an 'lazy_action'.
      m_type = asLazy(m_ctx, *convType, false);
    } else {
      m_type = *convType;
    }
    return;
  }

  // Templated function.
  if (typeParams) {
    const auto typeSet = getTypeSet(m_ctx, m_typeSubTable, typeParams->getTypes());
    if (!typeSet) {
      return;
    }
    if (const auto callInfo =
            m_ctx->getFuncTemplates()->getCallInfo(funcName, *typeSet, getOvOptions())) {
      if (n.isFork()) {
        m_type = asFuture(m_ctx, callInfo->resultType);
      } else if (n.isLazy()) {
        m_type = asLazy(m_ctx, callInfo->resultType, callInfo->isAction);
      } else {
        m_type = callInfo->resultType;
      }
    }
    return;
  }

  // Regular functions.
  if (const auto callInfo = inferFuncCall(funcName, argTypeSet)) {
    if (n.isFork()) {
      m_type = asFuture(m_ctx, callInfo->resultType);
    } else if (n.isLazy()) {
      m_type = asLazy(m_ctx, callInfo->resultType, callInfo->isAction);
    } else {
      m_type = callInfo->resultType;
    }
  }
}

auto TypeInferExpr::visit(const parse::ConditionalExprNode& n) -> void {
  // Run type-inference on the condition as it might declare consts.
  inferSubExpr(n[0]);

  auto branchTypes = std::vector<prog::sym::TypeId>{};
  branchTypes.reserve(2);

  const auto ifBranchType = inferSubExpr(n[1]);
  if (ifBranchType.isInfer() && !hasFlag<Flags::Aggressive>()) {
    return;
  }
  if (ifBranchType.isConcrete()) {
    branchTypes.push_back(ifBranchType);
  }

  const auto elseBranchType = inferSubExpr(n[2]);
  if (elseBranchType.isInfer() && !hasFlag<Flags::Aggressive>()) {
  }
  if (elseBranchType.isConcrete()) {
    branchTypes.push_back(elseBranchType);
  }

  // Find a type that both of the branches are convertible to.
  auto commonType = m_ctx->getProg()->findCommonType(branchTypes);
  if (commonType) {
    m_type = *commonType;
  }
}

auto TypeInferExpr::visit(const parse::ConstDeclExprNode& n) -> void {
  auto assignmentType = inferSubExpr(n[0]);
  setConstType(n.getId(), assignmentType);
  m_type = assignmentType;
}

auto TypeInferExpr::visit(const parse::IdExprNode& n) -> void {
  const auto name = getName(n.getId());

  // Check if this is a constant.
  m_type = inferConstType(n.getId());
  if (m_type.isConcrete()) {
    return;
  }

  // Templated function literal.
  if (n.getTypeParams()) {
    const auto typeSet = getTypeSet(m_ctx, m_typeSubTable, n.getTypeParams()->getTypes());
    if (!typeSet) {
      return;
    }
    const auto callInfo = m_ctx->getFuncTemplates()->getCallInfo(name, *typeSet, getOvOptions());
    if (callInfo) {
      m_type = m_ctx->getDelegates()->getDelegate(
          m_ctx, callInfo->isAction, callInfo->argumentTypes, callInfo->resultType);
    }
    return;
  }

  // Non-templated function literal.
  const auto funcs = m_ctx->getProg()->lookupFunc(name, getOvOptions(true));
  if (!funcs.empty() && !m_ctx->hasErrors()) {
    m_type = getDelegate(m_ctx, funcs[0]);
    return;
  }
}

auto TypeInferExpr::visit(const parse::FieldExprNode& n) -> void {
  // Check if the lhs is a type name token, in that case treat this as access to a 'static' field.
  auto getIdVisitor = GetIdentifier{false};
  n[0].accept(&getIdVisitor);
  auto identifier = getIdVisitor.getIdentifier();
  if (identifier && !getIdVisitor.isIntrinsic() &&
      isType(m_ctx, m_typeSubTable, getName(*identifier))) {
    const auto type =
        getOrInstType(m_ctx, m_typeSubTable, *identifier, getIdVisitor.getTypeParams());
    // If the type is an enum then any field on that enum will have that type.
    if (type && m_ctx->getProg()->getTypeDecl(*type).getKind() == prog::sym::TypeKind::Enum) {
      m_type = *type;
      return;
    }
  }

  // If not a static field then acces the field on the lhs expression.
  const auto lhsType = inferSubExpr(n[0]);
  if (!lhsType.isConcrete()) {
    return;
  }

  // Only structs are supported atm.
  if (m_ctx->getProg()->getTypeDecl(lhsType).getKind() != prog::sym::TypeKind::Struct) {
    return;
  }

  const auto& structDef    = std::get<prog::sym::StructDef>(m_ctx->getProg()->getTypeDef(lhsType));
  const auto& structFields = structDef.getFields();
  const auto& field        = structFields.lookup(getName(n.getId()));
  if (field) {
    m_type = structFields[*field].getType();
  }
}

auto TypeInferExpr::visit(const parse::GroupExprNode& n) -> void {
  for (auto i = 0U; i < n.getChildCount(); ++i) {
    m_type = inferSubExpr(n[i]);
  }
}

auto TypeInferExpr::visit(const parse::IndexExprNode& n) -> void {
  auto argTypes = std::vector<prog::sym::TypeId>{};
  argTypes.reserve(n.getChildCount());
  for (auto i = 0U; i < n.getChildCount(); ++i) {
    argTypes.push_back(inferSubExpr(n[i]));
  }
  const auto argTypeSet = prog::sym::TypeSet{std::move(argTypes)};
  const auto funcName   = prog::getFuncName(prog::Operator::SquareSquare);
  if (const auto callInfo = inferFuncCall(funcName, argTypeSet)) {
    m_type = callInfo->resultType;
  }
}

auto TypeInferExpr::visit(const parse::IntrinsicExprNode& /*unused*/) -> void {}

auto TypeInferExpr::visit(const parse::IsExprNode& n) -> void {
  if (n.hasId()) {
    // Register the type of the constant this declares.
    const auto constType = getOrInstType(m_ctx, m_typeSubTable, n.getType());
    if (constType) {
      setConstType(*n.getId(), *constType);
    }
  }

  // Expression itself always evaluates to a bool.
  m_type = m_ctx->getProg()->getBool();
}

auto TypeInferExpr::visit(const parse::LitExprNode& n) -> void {
  switch (n.getVal().getKind()) {
  case lex::TokenKind::LitInt:
    m_type = m_ctx->getProg()->getInt();
    break;
  case lex::TokenKind::LitLong:
    m_type = m_ctx->getProg()->getLong();
    break;
  case lex::TokenKind::LitFloat:
    m_type = m_ctx->getProg()->getFloat();
    break;
  case lex::TokenKind::LitBool:
    m_type = m_ctx->getProg()->getBool();
    break;
  case lex::TokenKind::LitString:
    m_type = m_ctx->getProg()->getString();
    break;
  case lex::TokenKind::LitChar:
    m_type = m_ctx->getProg()->getChar();
    break;
  default:
    break;
  }
}

auto TypeInferExpr::visit(const parse::ParenExprNode& n) -> void { m_type = inferSubExpr(n[0]); }

auto TypeInferExpr::visit(const parse::SwitchExprElseNode& /*unused*/) -> void {
  throw std::logic_error{"TypeInferExpr is not implemented for this node type"};
}

auto TypeInferExpr::visit(const parse::SwitchExprIfNode& /*unused*/) -> void {
  throw std::logic_error{"TypeInferExpr is not implemented for this node type"};
}

auto TypeInferExpr::visit(const parse::SwitchExprNode& n) -> void {
  auto branchTypes = std::vector<prog::sym::TypeId>{};
  branchTypes.reserve(n.getChildCount());

  for (auto i = 0U; i < n.getChildCount(); ++i) {
    const auto isElseClause = n.hasElse() && i == n.getChildCount() - 1;

    // Run type-inference on the conditions as they might declare consts.
    if (!isElseClause) {
      inferSubExpr(n[i][0]);
    }

    // Get types of the branches.
    auto branchType = inferSubExpr(n[i][isElseClause ? 0 : 1]);
    if (branchType.isInfer() && !hasFlag<Flags::Aggressive>()) {
      return;
    }
    if (branchType.isConcrete()) {
      branchTypes.push_back(branchType);
    }
  }

  // Find a type that all of the branches are convertible to.
  auto commonType = m_ctx->getProg()->findCommonType(branchTypes);
  if (commonType) {
    m_type = *commonType;
  }
}

auto TypeInferExpr::visit(const parse::UnaryExprNode& n) -> void {
  const auto& opToken = n.getOperator();
  const auto op       = getOperator(opToken);
  if (!op) {
    return;
  }
  auto argType = inferSubExpr(n[0]);
  if (const auto callInfo = inferFuncCall(prog::getFuncName(*op), {argType})) {
    m_type = callInfo->resultType;
  }
}

auto TypeInferExpr::visit(const parse::EnumDeclStmtNode& /*unused*/) -> void {
  throw std::logic_error{"TypeInferExpr is not implemented for this node type"};
}

auto TypeInferExpr::visit(const parse::ExecStmtNode& /*unused*/) -> void {
  throw std::logic_error{"TypeInferExpr is not implemented for this node type"};
}

auto TypeInferExpr::visit(const parse::FuncDeclStmtNode& /*unused*/) -> void {
  throw std::logic_error{"TypeInferExpr is not implemented for this node type"};
}

auto TypeInferExpr::visit(const parse::ImportStmtNode& /*unused*/) -> void {
  throw std::logic_error{"TypeInferExpr is not implemented for this node type"};
}

auto TypeInferExpr::visit(const parse::StructDeclStmtNode& /*unused*/) -> void {
  throw std::logic_error{"TypeInferExpr is not implemented for this node type"};
}

auto TypeInferExpr::visit(const parse::UnionDeclStmtNode& /*unused*/) -> void {
  throw std::logic_error{"TypeInferExpr is not implemented for this node type"};
}

auto TypeInferExpr::inferSubExpr(const parse::Node& n) -> prog::sym::TypeId {
  auto visitor = TypeInferExpr{m_ctx, m_typeSubTable, m_constTypes, m_flags};
  n.accept(&visitor);
  return visitor.getInferredType();
}

auto TypeInferExpr::inferDynCall(const parse::CallExprNode& n) -> prog::sym::TypeId {
  auto argTypes = std::vector<prog::sym::TypeId>{};
  argTypes.reserve(n.getChildCount());
  for (auto i = 0U; i < n.getChildCount(); ++i) {
    argTypes.push_back(inferSubExpr(n[i]));
  }

  // Call to a delegate type.
  if (m_ctx->getProg()->isDelegate(argTypes[0])) {
    const auto& delegateDef =
        std::get<prog::sym::DelegateDef>(m_ctx->getProg()->getTypeDef(argTypes[0]));
    auto result = delegateDef.getOutput();
    if (n.isFork()) {
      return asFuture(m_ctx, result);
    }
    if (n.isLazy()) {
      return asLazy(m_ctx, result, delegateDef.isAction());
    }
    return result;
  }

  // Call to a overloaded call operator.
  const auto argTypeSet = prog::sym::TypeSet{std::move(argTypes)};
  const auto funcName   = prog::getFuncName(prog::Operator::ParenParen);
  auto funcCallInfo     = inferFuncCall(funcName, argTypeSet);
  if (!funcCallInfo) {
    return prog::sym::TypeId::inferType();
  }

  if (n.isFork()) {
    return asFuture(m_ctx, funcCallInfo->resultType);
  }
  if (n.isLazy()) {
    return asLazy(m_ctx, funcCallInfo->resultType, funcCallInfo->isAction);
  }
  return funcCallInfo->resultType;
}

auto TypeInferExpr::inferFuncCall(const std::string& funcName, const prog::sym::TypeSet& argTypes)
    -> std::optional<CallInfo> {

  for (const auto& argType : argTypes) {
    if (argType.isInfer()) {
      return std::nullopt;
    }
  }

  // Attempt to get a return-type for a non-templated function.
  auto func = m_ctx->getProg()->lookupFunc(funcName, argTypes, getOvOptions());
  if (func) {
    const auto& funcDecl = m_ctx->getProg()->getFuncDecl(*func);
    if (funcDecl.getOutput().isInfer()) {
      return std::nullopt;
    }
    return CallInfo{funcDecl.getOutput(), funcDecl.isAction()};
  }

  // Attempt to get a return-type for a inferred templated function.
  auto inferredTemplFuncCallInfo =
      m_ctx->getFuncTemplates()->inferParamsAndGetCallInfo(funcName, argTypes, getOvOptions());
  if (inferredTemplFuncCallInfo) {
    return CallInfo{inferredTemplFuncCallInfo->resultType, inferredTemplFuncCallInfo->isAction};
  }

  return std::nullopt;
}

auto TypeInferExpr::setConstType(const lex::Token& constId, prog::sym::TypeId type) -> void {
  if (m_constTypes) {
    auto name = getName(constId);
    m_constTypes->insert({name, type});
  }
}

auto TypeInferExpr::inferConstType(const lex::Token& constId) -> prog::sym::TypeId {
  if (!m_constTypes) {
    return prog::sym::TypeId::inferType();
  }
  const auto itr = m_constTypes->find(getName(constId));
  if (itr == m_constTypes->end()) {
    return prog::sym::TypeId::inferType();
  }
  return itr->second;
}

} // namespace frontend::internal
