#include "internal/typeinfer_expr.hpp"
#include "internal/utilities.hpp"
#include "lex/token_payload_lit_bool.hpp"
#include "lex/token_payload_lit_int.hpp"
#include "parse/nodes.hpp"

namespace frontend::internal {

TypeInferExpr::TypeInferExpr(
    prog::Program* prog,
    FuncTemplateTable* funcTemplates,
    const TypeSubstitutionTable* typeSubTable,
    std::unordered_map<std::string, prog::sym::TypeId>* constTypes,
    bool aggressive) :
    m_prog{prog},
    m_funcTemplates{funcTemplates},
    m_typeSubTable{typeSubTable},
    m_constTypes{constTypes},
    m_aggressive{aggressive},
    m_type{prog::sym::TypeId::inferType()} {
  if (m_prog == nullptr) {
    throw std::invalid_argument{"Program cannot be null"};
  }
  if (m_funcTemplates == nullptr) {
    throw std::invalid_argument{"Function template table cannot be null"};
  }
  if (m_constTypes == nullptr) {
    throw std::invalid_argument{"ConstTypes cannot be null"};
  }
}

auto TypeInferExpr::getInferredType() const noexcept -> prog::sym::TypeId { return m_type; }

auto TypeInferExpr::visit(const parse::ErrorNode & /*unused*/) -> void {
  throw std::logic_error{"TypeInferExpr is not implemented for this node type"};
}

auto TypeInferExpr::visit(const parse::BinaryExprNode& n) -> void {
  const auto& opToken = n.getOperator();
  switch (opToken.getKind()) {
  case lex::TokenKind::OpAmpAmp:
  case lex::TokenKind::OpPipePipe: {
    m_type = m_prog->getBool();
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
  m_type = inferFuncCall(prog::getFuncName(*op), std::move(argTypes));
}

auto TypeInferExpr::visit(const parse::CallExprNode& n) -> void {
  const auto funcName = getName(n.getFunc());
  if (n.getTypeParams()) {
    const auto typeSet = getTypeSet(*m_prog, m_typeSubTable, n.getTypeParams()->getTypes());
    if (!typeSet) {
      return;
    }
    const auto retType = m_funcTemplates->getRetType(funcName, *typeSet);
    if (retType) {
      m_type = *retType;
    }
    return;
  }

  auto argTypes = std::vector<prog::sym::TypeId>{};
  argTypes.reserve(n.getChildCount());
  for (auto i = 0U; i < n.getChildCount(); ++i) {
    argTypes.push_back(inferSubExpr(n[i]));
  }
  m_type = inferFuncCall(funcName, std::move(argTypes));
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
  auto commonType = m_prog->findCommonType(branchTypes);
  if (commonType) {
    m_type = *commonType;
  }
}

auto TypeInferExpr::visit(const parse::ConstDeclExprNode& n) -> void {
  auto assignmentType = inferSubExpr(n[0]);
  setConstType(n.getId(), assignmentType);
}

auto TypeInferExpr::visit(const parse::ConstExprNode& n) -> void {
  m_type = inferConstType(n.getId());
}

auto TypeInferExpr::visit(const parse::FieldExprNode& n) -> void {
  const auto lhsType = inferSubExpr(n[0]);
  if (!lhsType.isConcrete()) {
    return;
  }

  // Only structs are supported atm.
  if (m_prog->getTypeDecl(lhsType).getKind() != prog::sym::TypeKind::UserStruct) {
    return;
  }

  const auto& structDef    = std::get<prog::sym::StructDef>(m_prog->getTypeDef(lhsType));
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

auto TypeInferExpr::visit(const parse::IsExprNode& n) -> void {
  // Register the type of the constant this declares.
  const auto constType = getType(*m_prog, m_typeSubTable, getName(n.getType()));
  if (constType) {
    setConstType(n.getId(), *constType);
  }

  // Expression itself always evaluates to a bool.
  m_type = m_prog->getBool();
}

auto TypeInferExpr::visit(const parse::LitExprNode& n) -> void {
  switch (n.getVal().getKind()) {
  case lex::TokenKind::LitInt: {
    m_type = m_prog->getInt();
    break;
  }
  case lex::TokenKind::LitFloat: {
    m_type = m_prog->getFloat();
    break;
  }
  case lex::TokenKind::LitBool: {
    m_type = m_prog->getBool();
    break;
  }
  case lex::TokenKind::LitString: {
    m_type = m_prog->getString();
    break;
  }
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
  auto commonType = m_prog->findCommonType(branchTypes);
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
  m_type       = inferFuncCall(prog::getFuncName(*op), {argType});
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
  auto visitor = TypeInferExpr{m_prog, m_funcTemplates, m_typeSubTable, m_constTypes, m_aggressive};
  n.accept(&visitor);
  return visitor.getInferredType();
}

auto TypeInferExpr::inferFuncCall(
    const std::string& funcName, std::vector<prog::sym::TypeId> argTypes) -> prog::sym::TypeId {
  for (const auto& argType : argTypes) {
    if (argType.isInfer()) {
      return prog::sym::TypeId::inferType();
    }
  }
  auto func = m_prog->lookupFunc(funcName, prog::sym::TypeSet{std::move(argTypes)}, -1);
  if (func) {
    return m_prog->getFuncDecl(*func).getOutput();
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

} // namespace frontend::internal
