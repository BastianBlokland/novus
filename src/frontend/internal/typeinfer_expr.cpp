#include "internal/typeinfer_expr.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/context.hpp"
#include "internal/get_identifier.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"
#include <cassert>

namespace frontend::internal {

TypeInferExpr::TypeInferExpr(
    Context* context,
    const TypeSubstitutionTable* typeSubTable,
    std::unordered_map<std::string, prog::sym::TypeId>* constTypes,
    bool aggressive) :
    m_context{context},
    m_typeSubTable{typeSubTable},
    m_constTypes{constTypes},
    m_aggressive{aggressive},
    m_type{prog::sym::TypeId::inferType()} {

  if (m_context == nullptr) {
    throw std::invalid_argument{"Context cannot be null"};
  }
}

auto TypeInferExpr::getInferredType() const noexcept -> prog::sym::TypeId { return m_type; }

auto TypeInferExpr::visit(const parse::CommentNode & /*unused*/) -> void {
  throw std::logic_error{"TypeInferExpr is not implemented for this node type"};
}

auto TypeInferExpr::visit(const parse::ErrorNode & /*unused*/) -> void {
  throw std::logic_error{"TypeInferExpr is not implemented for this node type"};
}

auto TypeInferExpr::visit(const parse::AnonFuncExprNode& n) -> void {
  auto funcInput = getFuncInput(m_context, m_typeSubTable, n);
  if (!funcInput) {
    assert(m_context->hasErrors());
    return;
  }

  const auto retType = inferRetType(m_context, m_typeSubTable, n, *funcInput, m_constTypes, true);
  m_type             = m_context->getDelegates()->getDelegate(m_context, *funcInput, retType);
}

auto TypeInferExpr::visit(const parse::BinaryExprNode& n) -> void {
  const auto& opToken = n.getOperator();
  switch (opToken.getKind()) {
  case lex::TokenKind::OpAmpAmp:
  case lex::TokenKind::OpPipePipe: {
    m_type = m_context->getProg()->getBool();
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
  m_type                = inferFuncCall(prog::getFuncName(*op), argTypeSet, true);
}

auto TypeInferExpr::visit(const parse::CallExprNode& n) -> void {
  auto getIdVisitor = GetIdentifier{};
  n[0].accept(&getIdVisitor);
  auto* instance  = getIdVisitor.getInstance();
  auto identifier = getIdVisitor.getIdentifier();
  auto typeParams = getIdVisitor.getTypeParams();

  // Dynamic call.
  if (!identifier || m_constTypes->find(getName(*identifier)) != m_constTypes->end() ||
      (instance != nullptr && !isFuncOrConv(m_context, getName(*identifier)))) {
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
  auto convType = getOrInstType(m_context, m_typeSubTable, nameToken, typeParams, argTypeSet);
  if (convType) {
    m_type = *convType;
    return;
  }

  // Templated function.
  if (typeParams) {
    const auto typeSet = getTypeSet(m_context, m_typeSubTable, typeParams->getTypes());
    if (!typeSet) {
      return;
    }
    const auto retType = m_context->getFuncTemplates()->getRetType(funcName, *typeSet);
    if (retType) {
      m_type = *retType;
    }
    return;
  }

  // Regular functions.
  m_type = inferFuncCall(funcName, argTypeSet, true);
}

auto TypeInferExpr::visit(const parse::ConditionalExprNode& n) -> void {
  // Run type-inference on the condition as it might declare consts.
  inferSubExpr(n[0]);

  auto branchTypes = std::vector<prog::sym::TypeId>{};
  branchTypes.reserve(2);

  const auto ifBranchType = inferSubExpr(n[1]);
  if (ifBranchType.isInfer() && !m_aggressive) {
    return;
  }
  if (ifBranchType.isConcrete()) {
    branchTypes.push_back(ifBranchType);
  }

  const auto elseBranchType = inferSubExpr(n[2]);
  if (elseBranchType.isInfer() && !m_aggressive) {
  }
  if (elseBranchType.isConcrete()) {
    branchTypes.push_back(elseBranchType);
  }

  // Find a type that both of the branches are convertible to.
  auto commonType = m_context->getProg()->findCommonType(branchTypes);
  if (commonType) {
    m_type = *commonType;
  }
}

auto TypeInferExpr::visit(const parse::ConstDeclExprNode& n) -> void {
  auto assignmentType = inferSubExpr(n[0]);
  setConstType(n.getId(), assignmentType);
}

auto TypeInferExpr::visit(const parse::IdExprNode& n) -> void {
  const auto name = getName(n.getId());

  // Templated function literal.
  if (n.getTypeParams()) {
    const auto typeSet = getTypeSet(m_context, m_typeSubTable, n.getTypeParams()->getTypes());
    if (!typeSet) {
      return;
    }
    const auto instances = m_context->getFuncTemplates()->instantiate(name, *typeSet);
    if (!instances.empty() && !m_context->hasErrors()) {
      const auto funcDecl = m_context->getProg()->getFuncDecl(*instances[0]->getFunc());
      m_type              = m_context->getDelegates()->getDelegate(
          m_context, funcDecl.getInput(), funcDecl.getOutput());
    }
    return;
  }

  // Non-templated function literal.
  const auto funcs = m_context->getProg()->lookupFuncs(name);
  if (!funcs.empty() && !m_context->hasErrors()) {
    const auto& funcDecl = m_context->getProg()->getFuncDecl(funcs[0]);
    if (funcDecl.getOutput().isConcrete()) {
      m_type = m_context->getDelegates()->getDelegate(
          m_context, funcDecl.getInput(), funcDecl.getOutput());
    }
    return;
  }

  // If its not a function literal treat it as a constant.
  m_type = inferConstType(n.getId());
}

auto TypeInferExpr::visit(const parse::FieldExprNode& n) -> void {
  const auto lhsType = inferSubExpr(n[0]);
  if (!lhsType.isConcrete()) {
    return;
  }

  // Only structs are supported atm.
  if (m_context->getProg()->getTypeDecl(lhsType).getKind() != prog::sym::TypeKind::UserStruct) {
    return;
  }

  const auto& structDef = std::get<prog::sym::StructDef>(m_context->getProg()->getTypeDef(lhsType));
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
  m_type                = inferFuncCall(funcName, argTypeSet, false);
}

auto TypeInferExpr::visit(const parse::IsExprNode& n) -> void {
  // Register the type of the constant this declares.
  const auto constType = getOrInstType(m_context, m_typeSubTable, n.getType());
  if (constType) {
    setConstType(n.getId(), *constType);
  }

  // Expression itself always evaluates to a bool.
  m_type = m_context->getProg()->getBool();
}

auto TypeInferExpr::visit(const parse::LitExprNode& n) -> void {
  switch (n.getVal().getKind()) {
  case lex::TokenKind::LitInt:
    m_type = m_context->getProg()->getInt();
    break;
  case lex::TokenKind::LitFloat:
    m_type = m_context->getProg()->getFloat();
    break;
  case lex::TokenKind::LitBool:
    m_type = m_context->getProg()->getBool();
    break;
  case lex::TokenKind::LitString:
    m_type = m_context->getProg()->getString();
    break;
  case lex::TokenKind::LitChar:
    m_type = m_context->getProg()->getChar();
    break;
  default:
    break;
  }
}

auto TypeInferExpr::visit(const parse::ParenExprNode& n) -> void { m_type = inferSubExpr(n[0]); }

auto TypeInferExpr::visit(const parse::SwitchExprElseNode & /*unused*/) -> void {
  throw std::logic_error{"TypeInferExpr is not implemented for this node type"};
}

auto TypeInferExpr::visit(const parse::SwitchExprIfNode & /*unused*/) -> void {
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
    if (branchType.isInfer() && !m_aggressive) {
      return;
    }
    if (branchType.isConcrete()) {
      branchTypes.push_back(branchType);
    }
  }

  // Find a type that all of the branches are convertible to.
  auto commonType = m_context->getProg()->findCommonType(branchTypes);
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
  m_type       = inferFuncCall(prog::getFuncName(*op), {argType}, false);
}

auto TypeInferExpr::visit(const parse::EnumDeclStmtNode & /*unused*/) -> void {
  throw std::logic_error{"TypeInferExpr is not implemented for this node type"};
}

auto TypeInferExpr::visit(const parse::ExecStmtNode & /*unused*/) -> void {
  throw std::logic_error{"TypeInferExpr is not implemented for this node type"};
}

auto TypeInferExpr::visit(const parse::FuncDeclStmtNode & /*unused*/) -> void {
  throw std::logic_error{"TypeInferExpr is not implemented for this node type"};
}

auto TypeInferExpr::visit(const parse::StructDeclStmtNode & /*unused*/) -> void {
  throw std::logic_error{"TypeInferExpr is not implemented for this node type"};
}

auto TypeInferExpr::visit(const parse::UnionDeclStmtNode & /*unused*/) -> void {
  throw std::logic_error{"TypeInferExpr is not implemented for this node type"};
}

auto TypeInferExpr::inferSubExpr(const parse::Node& n) -> prog::sym::TypeId {
  auto visitor = TypeInferExpr{m_context, m_typeSubTable, m_constTypes, m_aggressive};
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
  if (m_context->getProg()->isDelegate(argTypes[0])) {
    const auto& delegateDef =
        std::get<prog::sym::DelegateDef>(m_context->getProg()->getTypeDef(argTypes[0]));
    return delegateDef.getOutput();
  }

  // Call to a overloaded call operator.
  const auto argTypeSet = prog::sym::TypeSet{std::move(argTypes)};
  const auto funcName   = prog::getFuncName(prog::Operator::ParenParen);
  return inferFuncCall(funcName, argTypeSet, false);
}

auto TypeInferExpr::inferFuncCall(
    const std::string& funcName, const prog::sym::TypeSet& argTypes, bool allowConvOnFirstArg)
    -> prog::sym::TypeId {
  for (const auto& argType : argTypes) {
    if (argType.isInfer()) {
      return prog::sym::TypeId::inferType();
    }
  }

  // Attempt to get a return-type for a non-templated function.
  auto func = m_context->getProg()->lookupFunc(funcName, argTypes, -1, allowConvOnFirstArg);
  if (func) {
    return m_context->getProg()->getFuncDecl(*func).getOutput();
  }

  // Attempt to get a return-type for a inferred templated function.
  auto retTypeForInferredTemplFunc =
      m_context->getFuncTemplates()->inferParamsAndGetRetType(funcName, argTypes);
  if (retTypeForInferredTemplFunc) {
    return *retTypeForInferredTemplFunc;
  }

  return prog::sym::TypeId::inferType();
}

auto TypeInferExpr::setConstType(const lex::Token& constId, prog::sym::TypeId type) -> void {
  auto name = getName(constId);
  m_constTypes->insert({name, type});
}

auto TypeInferExpr::inferConstType(const lex::Token& constId) -> prog::sym::TypeId {
  const auto itr = m_constTypes->find(getName(constId));
  if (itr == m_constTypes->end()) {
    return prog::sym::TypeId::inferType();
  }
  return itr->second;
}

auto TypeInferExpr::isType(const std::string& name) const -> bool {
  return m_context->getProg()->lookupType(name) || m_context->getTypeTemplates()->hasType(name);
}

} // namespace frontend::internal
