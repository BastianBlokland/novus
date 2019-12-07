#include "gen_expr.hpp"
#include "prog/expr/nodes.hpp"
#include "utilities.hpp"
#include <stdexcept>
#include <vector>

namespace backend::internal {

GenExpr::GenExpr(const prog::Program& program, Builder* builder) :
    m_program{program}, m_builder{builder} {}

auto GenExpr::visit(const prog::expr::AssignExprNode& n) -> void {
  // Expression.
  genSubExpr(n[0]);

  // Assign op.
  const auto constId = getConstId(n.getConst());
  m_builder->addStoreConst(constId);
}

auto GenExpr::visit(const prog::expr::SwitchExprNode& n) -> void {
  const auto& conditions = n.getConditions();
  const auto& branches   = n.getBranches();

  // Generate labels for jumping.
  auto condBranchesLabels = std::vector<std::string>{};
  for (auto i = 0U; i < conditions.size(); ++i) {
    condBranchesLabels.push_back(m_builder->generateLabel());
  }
  const auto endLabel = m_builder->generateLabel();

  // Jump for the 'if' cases and fall-through for the else cases.
  for (auto i = 0U; i < conditions.size(); ++i) {
    genSubExpr(*conditions[i]);
    m_builder->addJumpIf(condBranchesLabels[i]);
  }

  // If all conditions where false we execute the else branch.
  genSubExpr(*branches.back());
  m_builder->addJump(endLabel);

  // Generate code for the 'if' branches.
  for (auto i = 0U; i < conditions.size(); ++i) {
    m_builder->label(condBranchesLabels[i]);
    genSubExpr(*branches[i]);

    // No need for a jump for the last.
    if (i != conditions.size() - 1) {
      m_builder->addJump(endLabel);
    }
  }

  m_builder->label(endLabel);
}

auto GenExpr::visit(const prog::expr::CallExprNode& n) -> void {
  const auto& funcDecl = m_program.getFuncDecl(n.getFunc());
  if (funcDecl.getKind() == prog::sym::FuncKind::MakeUnion) {
    // Union is an exception where the type-id needs to be on the stack before the argument.
    auto type = n[0].getType();
    m_builder->addLoadLitInt(static_cast<int32_t>(type.getNum()));
  }

  // Push the arguments on the stack.
  for (auto i = 0U; i < n.getChildCount(); ++i) {
    genSubExpr(n[i]);
  }

  // Either call the user function or the appropriate build-in instruction.
  switch (funcDecl.getKind()) {
  case prog::sym::FuncKind::User:
    m_builder->addCall(getLabel(funcDecl.getId()));
    break;

  case prog::sym::FuncKind::AddInt:
    m_builder->addAddInt();
    break;
  case prog::sym::FuncKind::SubInt:
    m_builder->addSubInt();
    break;
  case prog::sym::FuncKind::MulInt:
    m_builder->addMulInt();
    break;
  case prog::sym::FuncKind::DivInt:
    m_builder->addDivInt();
    break;
  case prog::sym::FuncKind::RemInt:
    m_builder->addRemInt();
    break;
  case prog::sym::FuncKind::NegateInt:
    m_builder->addNegInt();
    break;
  case prog::sym::FuncKind::CheckEqInt:
    m_builder->addCheckEqInt();
    break;
  case prog::sym::FuncKind::CheckNEqInt:
    m_builder->addCheckEqInt();
    m_builder->addLogicInvInt();
    break;
  case prog::sym::FuncKind::CheckLeInt:
    m_builder->addCheckLeInt();
    break;
  case prog::sym::FuncKind::CheckLeEqInt:
    m_builder->addCheckGtInt();
    m_builder->addLogicInvInt();
    break;
  case prog::sym::FuncKind::CheckGtInt:
    m_builder->addCheckGtInt();
    break;
  case prog::sym::FuncKind::CheckGtEqInt:
    m_builder->addCheckLeInt();
    m_builder->addLogicInvInt();
    break;

  case prog::sym::FuncKind::AddFloat:
    m_builder->addAddFloat();
    break;
  case prog::sym::FuncKind::SubFloat:
    m_builder->addSubFloat();
    break;
  case prog::sym::FuncKind::MulFloat:
    m_builder->addMulFloat();
    break;
  case prog::sym::FuncKind::DivFloat:
    m_builder->addDivFloat();
    break;
  case prog::sym::FuncKind::NegateFloat:
    m_builder->addNegFloat();
    break;
  case prog::sym::FuncKind::CheckEqFloat:
    m_builder->addCheckEqFloat();
    break;
  case prog::sym::FuncKind::CheckNEqFloat:
    m_builder->addCheckEqFloat();
    m_builder->addLogicInvInt();
    break;
  case prog::sym::FuncKind::CheckLeFloat:
    m_builder->addCheckLeFloat();
    break;
  case prog::sym::FuncKind::CheckLeEqFloat:
    m_builder->addCheckGtFloat();
    m_builder->addLogicInvInt();
    break;
  case prog::sym::FuncKind::CheckGtFloat:
    m_builder->addCheckGtFloat();
    break;
  case prog::sym::FuncKind::CheckGtEqFloat:
    m_builder->addCheckLeFloat();
    m_builder->addLogicInvInt();
    break;

  case prog::sym::FuncKind::InvBool:
    m_builder->addLogicInvInt();
    break;
  case prog::sym::FuncKind::CheckEqBool:
    m_builder->addCheckEqInt();
    break;
  case prog::sym::FuncKind::CheckNEqBool:
    m_builder->addCheckEqInt();
    m_builder->addLogicInvInt();
    break;

  case prog::sym::FuncKind::AddString:
    m_builder->addAddString();
    break;
  case prog::sym::FuncKind::CheckEqString:
    m_builder->addCheckEqString();
    break;
  case prog::sym::FuncKind::CheckNEqString:
    m_builder->addCheckEqString();
    m_builder->addLogicInvInt();
    break;

  case prog::sym::FuncKind::ConvIntFloat:
    m_builder->addConvIntFloat();
    break;
  case prog::sym::FuncKind::ConvFloatInt:
    m_builder->addConvFloatInt();
    break;
  case prog::sym::FuncKind::ConvIntString:
    m_builder->addConvIntString();
    break;
  case prog::sym::FuncKind::ConvFloatString:
    m_builder->addConvFloatString();
    break;
  case prog::sym::FuncKind::ConvBoolString:
    m_builder->addConvBoolString();
    break;

  case prog::sym::FuncKind::MakeStruct: {
    auto fieldCount = n.getChildCount();
    if (fieldCount > std::numeric_limits<uint8_t>::max()) {
      throw std::logic_error{"More then 256 fields in one struct are not supported"};
    }
    m_builder->addMakeStruct(static_cast<uint8_t>(fieldCount));
    break;
  }
  case prog::sym::FuncKind::MakeUnion: {
    // Unions are structs with 2 fields, first the type-id and then the value.
    // Note: The type-id is being pushed on the stack at the top of this function.
    m_builder->addMakeStruct(2);
    break;
  }

  case prog::sym::FuncKind::CheckEqUserType:
  case prog::sym::FuncKind::CheckNEqUserType:
    auto lhsType = n[0].getType();
    auto rhsType = n[1].getType();
    if (lhsType != rhsType) {
      throw std::logic_error{"User-type equality function requires args to have the same type"};
    }
    m_builder->addCall(getUserTypeEqLabel(lhsType));
    if (funcDecl.getKind() == prog::sym::FuncKind::CheckNEqUserType) {
      m_builder->addLogicInvInt();
    }
    break;
  }
}

auto GenExpr::visit(const prog::expr::ConstExprNode& n) -> void {
  const auto constId = getConstId(n.getId());
  m_builder->addLoadConst(constId);
}

auto GenExpr::visit(const prog::expr::FieldExprNode& n) -> void {
  // Load the struct.
  genSubExpr(n[0]);

  // Load the field.
  const auto fieldId = getFieldId(n.getId());
  m_builder->addLoadStructField(fieldId);
}

auto GenExpr::visit(const prog::expr::GroupExprNode& n) -> void {
  for (auto i = 0U; i < n.getChildCount(); ++i) {
    genSubExpr(n[i]);
  }
}

auto GenExpr::visit(const prog::expr::UnionGetExprNode& n) -> void {
  // Load the union.
  genSubExpr(n[0]);

  // We need the union twice on the stack, once for the type check and once for getting the value.
  m_builder->addDup();

  const auto typeEqLabel = m_builder->generateLabel();

  // Test if the union is the correct type.
  m_builder->addLoadStructField(0);
  m_builder->addLoadLitInt(static_cast<int32_t>(n.getTargetType().getNum()));
  m_builder->addCheckEqInt();
  m_builder->addJumpIf(typeEqLabel);

  m_builder->addPop(); // Pop the extra union value from the stack (from the duplicate before).
  m_builder->addLoadLitInt(0); // Load false.

  m_builder->label(typeEqLabel);

  // Store the union value as a const and load 'true' on the stack.
  const auto constId = getConstId(n.getConst());
  m_builder->addLoadStructField(1);
  m_builder->addStoreConst(constId);

  m_builder->addLoadLitInt(1); // Load true.
}

auto GenExpr::visit(const prog::expr::LitBoolNode& n) -> void {
  m_builder->addLoadLitInt(n.getVal() ? 1U : 0U);
}

auto GenExpr::visit(const prog::expr::LitFloatNode& n) -> void {
  m_builder->addLoadLitFloat(n.getVal());
}

auto GenExpr::visit(const prog::expr::LitIntNode& n) -> void {
  m_builder->addLoadLitInt(n.getVal());
}

auto GenExpr::visit(const prog::expr::LitStringNode& n) -> void {
  m_builder->addLoadLitString(n.getVal());
}

auto GenExpr::genSubExpr(const prog::expr::Node& n) -> void {
  auto genExpr = GenExpr{m_program, m_builder};
  n.accept(&genExpr);
}

} // namespace backend::internal
