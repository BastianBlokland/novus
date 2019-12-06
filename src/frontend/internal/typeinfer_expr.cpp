#include "internal/typeinfer_expr.hpp"
#include "internal/utilities.hpp"
#include "lex/token_payload_lit_bool.hpp"
#include "lex/token_payload_lit_int.hpp"
#include "parse/nodes.hpp"

namespace frontend::internal {

TypeInferExpr::TypeInferExpr(
    prog::Program* prog, std::unordered_map<std::string, prog::sym::TypeId>* constTypes) :
    m_prog{prog}, m_constTypes{constTypes}, m_type{prog::sym::TypeId::inferType()} {
  if (m_prog == nullptr) {
    throw std::invalid_argument{"Program cannot be null"};
  }
  if (m_constTypes == nullptr) {
    throw std::invalid_argument{"ConstTypes cannot be null"};
  }
}

auto TypeInferExpr::getType() const noexcept -> prog::sym::TypeId { return m_type; }

auto TypeInferExpr::visit(const parse::ErrorNode & /*unused*/) -> void {
  throw std::logic_error{"TypeInferExpr is not implemented for this node type"};
}

auto TypeInferExpr::visit(const parse::BinaryExprNode& n) -> void {
  const auto& opToken = n.getOperator();
  switch (opToken.getKind()) {
  case lex::TokenKind::OpAmpAmp:
  case lex::TokenKind::OpPipePipe: {
    auto boolType = m_prog->lookupType("bool");
    if (!boolType) {
      throw std::logic_error{"No 'bool' type present in type-table"};
    }
    m_type = *boolType;
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
  auto argTypes = std::vector<prog::sym::TypeId>{};
  argTypes.reserve(n.getChildCount());
  for (auto i = 0U; i < n.getChildCount(); ++i) {
    argTypes.push_back(inferSubExpr(n[i]));
  }
  m_type = inferFuncCall(getName(n.getFunc()), std::move(argTypes));
}

auto TypeInferExpr::visit(const parse::ConditionalExprNode& n) -> void {
  const auto ifBranchType = inferSubExpr(n[1]);
  if (ifBranchType.isConcrete()) {
    m_type = ifBranchType;
    return;
  }
  m_type = inferSubExpr(n[2]);
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

auto TypeInferExpr::visit(const parse::IsExprNode & /*unused*/) -> void {
  auto boolType = m_prog->lookupType("bool");
  if (!boolType) {
    throw std::logic_error{"No 'bool' type present in type-table"};
  }
  m_type = *boolType;
}

auto TypeInferExpr::visit(const parse::LitExprNode& n) -> void {
  switch (n.getVal().getKind()) {
  case lex::TokenKind::LitInt: {
    auto intType = m_prog->lookupType("int");
    if (!intType) {
      throw std::logic_error{"No 'int' type present in type-table"};
    }
    m_type = *intType;
    break;
  }
  case lex::TokenKind::LitFloat: {
    auto floatType = m_prog->lookupType("float");
    if (!floatType) {
      throw std::logic_error{"No 'float' type present in type-table"};
    }
    m_type = *floatType;
    break;
  }
  case lex::TokenKind::LitBool: {
    auto boolType = m_prog->lookupType("bool");
    if (!boolType) {
      throw std::logic_error{"No 'bool' type present in type-table"};
    }
    m_type = *boolType;
    break;
  }
  case lex::TokenKind::LitString: {
    auto stringType = m_prog->lookupType("string");
    if (!stringType) {
      throw std::logic_error{"No 'string' type present in type-table"};
    }
    m_type = *stringType;
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
  for (auto i = 0U; i < n.getChildCount(); ++i) {
    const auto isElseClause = i == n.getChildCount() - 1;
    auto branchType         = inferSubExpr(n[i][isElseClause ? 0 : 1]);

    // Because all branches have the same type we can stop when we successfully inferred one.
    if (branchType.isConcrete()) {
      m_type = branchType;
      return;
    }
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
  auto visitor = TypeInferExpr{m_prog, m_constTypes};
  n.accept(&visitor);
  return visitor.getType();
}

auto TypeInferExpr::inferFuncCall(
    const std::string& funcName, std::vector<prog::sym::TypeId> argTypes) -> prog::sym::TypeId {
  for (const auto& argType : argTypes) {
    if (argType.isInfer()) {
      return prog::sym::TypeId::inferType();
    }
  }
  auto func = m_prog->lookupFunc(funcName, prog::sym::Input{std::move(argTypes)}, -1);
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
