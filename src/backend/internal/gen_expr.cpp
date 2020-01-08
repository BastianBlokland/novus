#include "gen_expr.hpp"
#include "prog/expr/nodes.hpp"
#include "utilities.hpp"
#include <stdexcept>
#include <vector>

namespace backend::internal {

GenExpr::GenExpr(const prog::Program& program, Builder* builder, bool tail) :
    m_program{program}, m_builder{builder}, m_tail{tail} {}

auto GenExpr::visit(const prog::expr::AssignExprNode& n) -> void {
  // Expression.
  genSubExpr(n[0], false);

  // Duplicate the value as the store instruction will consume one.
  m_builder->addDup();

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
    genSubExpr(*conditions[i], false);
    m_builder->addJumpIf(condBranchesLabels[i]);
  }

  // If all conditions where false we execute the else branch.
  genSubExpr(*branches.back(), m_tail);
  m_builder->addJump(endLabel);

  // Generate code for the 'if' branches.
  for (auto i = 0U; i < conditions.size(); ++i) {
    m_builder->label(condBranchesLabels[i]);
    genSubExpr(*branches[i], m_tail);

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
    m_builder->addLoadLitInt(getUnionTypeId(m_program, n.getType(), n[0].getType()));
  }

  // Push the arguments on the stack.
  for (auto i = 0U; i < n.getChildCount(); ++i) {
    genSubExpr(n[i], false);
  }

  // Either call the user function or the appropriate build-in instruction.
  switch (funcDecl.getKind()) {
  case prog::sym::FuncKind::NoOp:
    // Make sure exactly one value is produced on the stack (insert placeholder if function takes no
    // arguments and add pops if function takes more then 1 argument).
    if (n.getChildCount() == 0) {
      m_builder->addLoadLitInt(0);
    } else {
      for (auto i = 1U; i < n.getChildCount(); ++i) {
        m_builder->addPop();
      }
    }
    break;
  case prog::sym::FuncKind::User:
    m_builder->addCall(getLabel(funcDecl.getId()), m_tail);
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
  case prog::sym::FuncKind::ShiftLeftInt:
    m_builder->addShiftLeftInt();
    break;
  case prog::sym::FuncKind::ShiftRightInt:
    m_builder->addShiftRightInt();
    break;
  case prog::sym::FuncKind::AndInt:
    m_builder->addAndInt();
    break;
  case prog::sym::FuncKind::OrInt:
    m_builder->addOrInt();
    break;
  case prog::sym::FuncKind::XorInt:
    m_builder->addXorInt();
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
  case prog::sym::FuncKind::LengthString:
    m_builder->addLengthString();
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
  case prog::sym::FuncKind::ConvCharString:
    m_builder->addConvCharString();
    break;
  case prog::sym::FuncKind::ConvIntChar:
    m_builder->addConvIntChar();
    break;

  case prog::sym::FuncKind::DefInt:
    m_builder->addLoadLitInt(0);
    break;
  case prog::sym::FuncKind::DefFloat:
    m_builder->addLoadLitFloat(.0);
    break;
  case prog::sym::FuncKind::DefBool:
    m_builder->addLoadLitInt(0);
    break;
  case prog::sym::FuncKind::DefString:
    m_builder->addLoadLitString("");
    break;

  case prog::sym::FuncKind::MakeStruct: {
    auto fieldCount = n.getChildCount();
    if (fieldCount == 0U) {
      // Empty structs are represented by the value 0 (avoids allocation).
      m_builder->addLoadLitInt(0);
      break;
    }
    if (fieldCount == 1U) {
      // Structs with one field are represented by the field only (avoids allocation).
      break;
    }

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
    auto invert = funcDecl.getKind() == prog::sym::FuncKind::CheckNEqUserType;
    m_builder->addCall(getUserTypeEqLabel(lhsType), m_tail && !invert);
    if (invert) {
      m_builder->addLogicInvInt();
    }
    break;
  }
}
auto GenExpr::visit(const prog::expr::CallDynExprNode& n) -> void {
  // Push the arguments on the stack.
  for (auto i = 1U; i < n.getChildCount(); ++i) {
    genSubExpr(n[i], false);
  }

  // Push the delegate on the stack.
  genSubExpr(n[0], false);

  // Invoke the delegate.
  m_builder->addCallDyn(m_tail);
}

auto GenExpr::visit(const prog::expr::ClosureNode& n) -> void {
  // Push the bound arguments on the stack.
  for (auto i = 0U; i < n.getChildCount(); ++i) {
    genSubExpr(n[i], false);
  }

  // Push the function instruction-pointer on the stack.
  const auto funcId = n.getFunc();
  m_builder->addLoadLitIp(getLabel(funcId));

  // Create a struct containing both the bound arguments and the function pointer.
  m_builder->addMakeStruct(n.getChildCount() + 1);
}

auto GenExpr::visit(const prog::expr::ConstExprNode& n) -> void {
  const auto constId = getConstId(n.getId());
  m_builder->addLoadConst(constId);
}

auto GenExpr::visit(const prog::expr::FieldExprNode& n) -> void {
  // Load the struct.
  genSubExpr(n[0], false);

  const auto& structType = m_program.getTypeDecl(n[0].getType());
  if (structType.getKind() != prog::sym::TypeKind::UserStruct) {
    throw std::logic_error{"Field expr node only works on struct types"};
  }
  const auto& structDef = std::get<prog::sym::StructDef>(m_program.getTypeDef(structType.getId()));
  if (structDef.getFields().getCount() == 0) {
    throw std::logic_error{"Cannot get a field on a struct without fields"};
  }
  if (structDef.getFields().getCount() == 1) {
    // Structs with one field are represented by the field only, so in this case the struct itself
    // is the value already.
    return;
  }

  // Load the field.
  const auto fieldId = getFieldId(n.getId());
  m_builder->addLoadStructField(fieldId);
}

auto GenExpr::visit(const prog::expr::GroupExprNode& n) -> void {
  for (auto i = 0U; i < n.getChildCount(); ++i) {
    const auto last = i == n.getChildCount() - 1;
    genSubExpr(n[i], m_tail && last);

    // For all but the last expression we ignore the result.
    if (!last) {
      m_builder->addPop();
    }
  }
}

auto GenExpr::visit(const prog::expr::UnionCheckExprNode& n) -> void {
  // Load the union.
  genSubExpr(n[0], false);

  // Test if the union is the correct type.
  m_builder->addLoadStructField(0);
  m_builder->addLoadLitInt(getUnionTypeId(m_program, n[0].getType(), n.getTargetType()));
  m_builder->addCheckEqInt();
}

auto GenExpr::visit(const prog::expr::UnionGetExprNode& n) -> void {
  // Load the union.
  genSubExpr(n[0], false);

  // We need the union twice on the stack, once for the type check and once for getting the value.
  m_builder->addDup();

  const auto typeEqLabel = m_builder->generateLabel();
  const auto endLabel    = m_builder->generateLabel();

  // Test if the union is the correct type.
  m_builder->addLoadStructField(0);
  m_builder->addLoadLitInt(getUnionTypeId(m_program, n[0].getType(), n.getTargetType()));
  m_builder->addCheckEqInt();
  m_builder->addJumpIf(typeEqLabel);

  m_builder->addPop(); // Pop the extra union value from the stack (from the duplicate before).
  m_builder->addLoadLitInt(0); // Load false.
  m_builder->addJump(endLabel);

  m_builder->label(typeEqLabel);

  // Store the union value as a const and load 'true' on the stack.
  const auto constId = getConstId(n.getConst());
  m_builder->addLoadStructField(1);
  m_builder->addStoreConst(constId);

  m_builder->addLoadLitInt(1); // Load true.

  m_builder->label(endLabel);
}

auto GenExpr::visit(const prog::expr::FailNode & /*unused*/) -> void { m_builder->addFail(); }

auto GenExpr::visit(const prog::expr::LitBoolNode& n) -> void {
  m_builder->addLoadLitInt(n.getVal() ? 1U : 0U);
}

auto GenExpr::visit(const prog::expr::LitFloatNode& n) -> void {
  m_builder->addLoadLitFloat(n.getVal());
}

auto GenExpr::visit(const prog::expr::LitFuncNode& n) -> void {
  const auto funcId = n.getFunc();
  m_builder->addLoadLitIp(getLabel(funcId));
}

auto GenExpr::visit(const prog::expr::LitIntNode& n) -> void {
  m_builder->addLoadLitInt(n.getVal());
}

auto GenExpr::visit(const prog::expr::LitStringNode& n) -> void {
  m_builder->addLoadLitString(n.getVal());
}

auto GenExpr::visit(const prog::expr::LitCharNode& n) -> void {
  m_builder->addLoadLitInt(n.getVal());
}

auto GenExpr::genSubExpr(const prog::expr::Node& n, bool tail) -> void {
  auto genExpr = GenExpr{m_program, m_builder, tail};
  n.accept(&genExpr);
}

} // namespace backend::internal
