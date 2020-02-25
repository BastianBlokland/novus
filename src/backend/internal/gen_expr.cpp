#include "gen_expr.hpp"
#include "prog/expr/nodes.hpp"
#include "utilities.hpp"
#include <stdexcept>
#include <vector>

namespace backend::internal {

GenExpr::GenExpr(
    const prog::Program& program,
    novasm::Assembler* asmb,
    const prog::sym::ConstDeclTable& constTable,
    std::optional<prog::sym::FuncId> curFunc,
    bool tail) :
    m_program{program}, m_asmb{asmb}, m_constTable{constTable}, m_curFunc{curFunc}, m_tail{tail} {}

auto GenExpr::visit(const prog::expr::AssignExprNode& n) -> void {
  // Expression.
  genSubExpr(n[0], false);

  // Duplicate the value as the store instruction will consume one.
  m_asmb->addDup();

  // Assign op.
  const auto constId = getConstOffset(m_constTable, n.getConst());
  m_asmb->addStackStore(constId);
}

auto GenExpr::visit(const prog::expr::SwitchExprNode& n) -> void {
  const auto& conditions = n.getConditions();
  const auto& branches   = n.getBranches();

  // Generate labels for jumping.
  auto condBranchesLabels = std::vector<std::string>{};
  for (auto i = 0U; i < conditions.size(); ++i) {
    condBranchesLabels.push_back(m_asmb->generateLabel());
  }
  const auto endLabel = m_asmb->generateLabel();

  // Jump for the 'if' cases and fall-through for the else cases.
  for (auto i = 0U; i < conditions.size(); ++i) {
    genSubExpr(*conditions[i], false);
    m_asmb->addJumpIf(condBranchesLabels[i]);
  }

  // If all conditions where false we execute the else branch.
  genSubExpr(*branches.back(), m_tail);
  m_asmb->addJump(endLabel);

  // Generate code for the 'if' branches.
  for (auto i = 0U; i < conditions.size(); ++i) {
    m_asmb->label(condBranchesLabels[i]);
    genSubExpr(*branches[i], m_tail);

    // No need for a jump for the last.
    if (i != conditions.size() - 1) {
      m_asmb->addJump(endLabel);
    }
  }

  m_asmb->label(endLabel);
}

auto GenExpr::visit(const prog::expr::CallExprNode& n) -> void {
  const auto& funcDecl = m_program.getFuncDecl(n.getFunc());
  if (funcDecl.getKind() == prog::sym::FuncKind::MakeUnion) {
    // Union is an exception where the type-id needs to be on the stack before the argument.
    m_asmb->addLoadLitInt(getUnionTypeId(m_program, n.getType(), n[0].getType()));
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
      m_asmb->addLoadLitInt(0);
    } else {
      for (auto i = 1U; i < n.getChildCount(); ++i) {
        m_asmb->addPop();
      }
    }
    break;
  case prog::sym::FuncKind::User:
    m_asmb->addCall(
        getLabel(funcDecl.getId()),
        funcDecl.getInput().getCount(),
        n.isFork() ? novasm::CallMode::Forked
                   : (m_tail ? novasm::CallMode::Tail : novasm::CallMode::Normal));
    break;

  case prog::sym::FuncKind::AddInt:
    m_asmb->addAddInt();
    break;
  case prog::sym::FuncKind::SubInt:
    m_asmb->addSubInt();
    break;
  case prog::sym::FuncKind::MulInt:
    m_asmb->addMulInt();
    break;
  case prog::sym::FuncKind::DivInt:
    m_asmb->addDivInt();
    break;
  case prog::sym::FuncKind::RemInt:
    m_asmb->addRemInt();
    break;
  case prog::sym::FuncKind::NegateInt:
    m_asmb->addNegInt();
    break;
  case prog::sym::FuncKind::IncrementInt:
    m_asmb->addLoadLitInt(1);
    m_asmb->addAddInt();
    break;
  case prog::sym::FuncKind::DecrementInt:
    m_asmb->addLoadLitInt(1);
    m_asmb->addSubInt();
    break;
  case prog::sym::FuncKind::CheckEqInt:
    m_asmb->addCheckEqInt();
    break;
  case prog::sym::FuncKind::CheckNEqInt:
    m_asmb->addCheckEqInt();
    m_asmb->addLogicInvInt();
    break;
  case prog::sym::FuncKind::CheckLeInt:
    m_asmb->addCheckLeInt();
    break;
  case prog::sym::FuncKind::CheckLeEqInt:
    m_asmb->addCheckGtInt();
    m_asmb->addLogicInvInt();
    break;
  case prog::sym::FuncKind::CheckGtInt:
    m_asmb->addCheckGtInt();
    break;
  case prog::sym::FuncKind::CheckGtEqInt:
    m_asmb->addCheckLeInt();
    m_asmb->addLogicInvInt();
    break;

  case prog::sym::FuncKind::AddLong:
    m_asmb->addAddLong();
    break;
  case prog::sym::FuncKind::SubLong:
    m_asmb->addSubLong();
    break;
  case prog::sym::FuncKind::MulLong:
    m_asmb->addMulLong();
    break;
  case prog::sym::FuncKind::DivLong:
    m_asmb->addDivLong();
    break;
  case prog::sym::FuncKind::RemLong:
    m_asmb->addRemLong();
    break;
  case prog::sym::FuncKind::NegateLong:
    m_asmb->addNegLong();
    break;
  case prog::sym::FuncKind::IncrementLong:
    m_asmb->addLoadLitLong(1);
    m_asmb->addAddLong();
    break;
  case prog::sym::FuncKind::DecrementLong:
    m_asmb->addLoadLitLong(1);
    m_asmb->addSubLong();
    break;
  case prog::sym::FuncKind::CheckEqLong:
    m_asmb->addCheckEqLong();
    break;
  case prog::sym::FuncKind::CheckNEqLong:
    m_asmb->addCheckEqLong();
    m_asmb->addLogicInvInt();
    break;
  case prog::sym::FuncKind::CheckLeLong:
    m_asmb->addCheckLeLong();
    break;
  case prog::sym::FuncKind::CheckLeEqLong:
    m_asmb->addCheckGtLong();
    m_asmb->addLogicInvInt();
    break;
  case prog::sym::FuncKind::CheckGtLong:
    m_asmb->addCheckGtLong();
    break;
  case prog::sym::FuncKind::CheckGtEqLong:
    m_asmb->addCheckLeLong();
    m_asmb->addLogicInvInt();
    break;

  case prog::sym::FuncKind::AddFloat:
    m_asmb->addAddFloat();
    break;
  case prog::sym::FuncKind::SubFloat:
    m_asmb->addSubFloat();
    break;
  case prog::sym::FuncKind::MulFloat:
    m_asmb->addMulFloat();
    break;
  case prog::sym::FuncKind::DivFloat:
    m_asmb->addDivFloat();
    break;
  case prog::sym::FuncKind::ModFloat:
    m_asmb->addModFloat();
    break;
  case prog::sym::FuncKind::PowFloat:
    m_asmb->addPowFloat();
    break;
  case prog::sym::FuncKind::SqrtFloat:
    m_asmb->addSqrtFloat();
    break;
  case prog::sym::FuncKind::SinFloat:
    m_asmb->addSinFloat();
    break;
  case prog::sym::FuncKind::CosFloat:
    m_asmb->addCosFloat();
    break;
  case prog::sym::FuncKind::TanFloat:
    m_asmb->addTanFloat();
    break;
  case prog::sym::FuncKind::ASinFloat:
    m_asmb->addASinFloat();
    break;
  case prog::sym::FuncKind::ACosFloat:
    m_asmb->addACosFloat();
    break;
  case prog::sym::FuncKind::ATanFloat:
    m_asmb->addATanFloat();
    break;
  case prog::sym::FuncKind::ATan2Float:
    m_asmb->addATan2Float();
    break;
  case prog::sym::FuncKind::NegateFloat:
    m_asmb->addNegFloat();
    break;
  case prog::sym::FuncKind::IncrementFloat:
    m_asmb->addLoadLitFloat(1.0F);
    m_asmb->addAddFloat();
    break;
  case prog::sym::FuncKind::DecrementFloat:
    m_asmb->addLoadLitFloat(1.0F);
    m_asmb->addSubFloat();
    break;
  case prog::sym::FuncKind::ShiftLeftInt:
    m_asmb->addShiftLeftInt();
    break;
  case prog::sym::FuncKind::ShiftRightInt:
    m_asmb->addShiftRightInt();
    break;
  case prog::sym::FuncKind::AndInt:
    m_asmb->addAndInt();
    break;
  case prog::sym::FuncKind::OrInt:
    m_asmb->addOrInt();
    break;
  case prog::sym::FuncKind::XorInt:
    m_asmb->addXorInt();
    break;
  case prog::sym::FuncKind::InvInt:
    m_asmb->addInvInt();
    break;

  case prog::sym::FuncKind::CheckEqFloat:
    m_asmb->addCheckEqFloat();
    break;
  case prog::sym::FuncKind::CheckNEqFloat:
    m_asmb->addCheckEqFloat();
    m_asmb->addLogicInvInt();
    break;
  case prog::sym::FuncKind::CheckLeFloat:
    m_asmb->addCheckLeFloat();
    break;
  case prog::sym::FuncKind::CheckLeEqFloat:
    m_asmb->addCheckGtFloat();
    m_asmb->addLogicInvInt();
    break;
  case prog::sym::FuncKind::CheckGtFloat:
    m_asmb->addCheckGtFloat();
    break;
  case prog::sym::FuncKind::CheckGtEqFloat:
    m_asmb->addCheckLeFloat();
    m_asmb->addLogicInvInt();
    break;

  case prog::sym::FuncKind::InvBool:
    m_asmb->addLogicInvInt();
    break;
  case prog::sym::FuncKind::CheckEqBool:
    m_asmb->addCheckEqInt();
    break;
  case prog::sym::FuncKind::CheckNEqBool:
    m_asmb->addCheckEqInt();
    m_asmb->addLogicInvInt();
    break;

  case prog::sym::FuncKind::AddString:
    m_asmb->addAddString();
    break;
  case prog::sym::FuncKind::LengthString:
    m_asmb->addLengthString();
    break;
  case prog::sym::FuncKind::IndexString:
    m_asmb->addIndexString();
    break;
  case prog::sym::FuncKind::SliceString:
    m_asmb->addSliceString();
    break;
  case prog::sym::FuncKind::CheckEqString:
    m_asmb->addCheckEqString();
    break;
  case prog::sym::FuncKind::CheckNEqString:
    m_asmb->addCheckEqString();
    m_asmb->addLogicInvInt();
    break;

  case prog::sym::FuncKind::IncrementChar:
    m_asmb->addLoadLitInt(1);
    m_asmb->addAddInt();
    m_asmb->addConvIntChar();
    break;
  case prog::sym::FuncKind::DecrementChar:
    m_asmb->addLoadLitInt(1);
    m_asmb->addSubInt();
    m_asmb->addConvIntChar();
    break;

  case prog::sym::FuncKind::ConvIntLong:
    m_asmb->addConvIntLong();
    break;
  case prog::sym::FuncKind::ConvIntFloat:
    m_asmb->addConvIntFloat();
    break;
  case prog::sym::FuncKind::ConvLongInt:
    m_asmb->addConvLongInt();
    break;
  case prog::sym::FuncKind::ConvFloatInt:
    m_asmb->addConvFloatInt();
    break;
  case prog::sym::FuncKind::ConvIntString:
    m_asmb->addConvIntString();
    break;
  case prog::sym::FuncKind::ConvLongString:
    m_asmb->addConvLongString();
    break;
  case prog::sym::FuncKind::ConvFloatString:
    m_asmb->addConvFloatString();
    break;
  case prog::sym::FuncKind::ConvBoolString:
    m_asmb->addConvBoolString();
    break;
  case prog::sym::FuncKind::ConvCharString:
    m_asmb->addConvCharString();
    break;
  case prog::sym::FuncKind::ConvIntChar:
    m_asmb->addConvIntChar();
    break;

  case prog::sym::FuncKind::DefInt:
    m_asmb->addLoadLitInt(0);
    break;
  case prog::sym::FuncKind::DefLong:
    m_asmb->addLoadLitLong(0);
    break;
  case prog::sym::FuncKind::DefFloat:
    m_asmb->addLoadLitFloat(.0);
    break;
  case prog::sym::FuncKind::DefBool:
    m_asmb->addLoadLitInt(0);
    break;
  case prog::sym::FuncKind::DefString:
    m_asmb->addLoadLitString("");
    break;

  case prog::sym::FuncKind::MakeStruct: {
    auto fieldCount = n.getChildCount();
    if (fieldCount == 0U) {
      // Empty structs are represented by the value 0 (avoids allocation).
      m_asmb->addLoadLitInt(0);
      break;
    }
    if (fieldCount == 1U) {
      // Structs with one field are represented by the field only (avoids allocation).
      break;
    }

    if (fieldCount > std::numeric_limits<uint8_t>::max()) {
      throw std::logic_error{"More then 256 fields in one struct are not supported"};
    }
    m_asmb->addMakeStruct(static_cast<uint8_t>(fieldCount));
    break;
  }
  case prog::sym::FuncKind::MakeUnion: {
    // Unions are structs with 2 fields, first the type-id and then the value.
    // Note: The type-id is being pushed on the stack at the top of this function.
    m_asmb->addMakeStruct(2);
    break;
  }

  case prog::sym::FuncKind::FutureWaitNano: {
    m_asmb->addFutureWaitNano();
    break;
  }
  case prog::sym::FuncKind::FutureBlock: {
    m_asmb->addFutureBlock();
    break;
  }

  case prog::sym::FuncKind::CheckEqUserType:
  case prog::sym::FuncKind::CheckNEqUserType: {
    auto lhsType = n[0].getType();
    auto rhsType = n[1].getType();
    if (lhsType != rhsType) {
      throw std::logic_error{"User-type equality function requires args to have the same type"};
    }
    auto invert = funcDecl.getKind() == prog::sym::FuncKind::CheckNEqUserType;
    m_asmb->addCall(
        getUserTypeEqLabel(lhsType),
        2,
        (m_tail && !invert) ? novasm::CallMode::Tail : novasm::CallMode::Normal);
    if (invert) {
      m_asmb->addLogicInvInt();
    }
    break;
  }

  // Platform actions:
  case prog::sym::FuncKind::ActionConWriteChar:
    m_asmb->addPCall(novasm::PCallCode::ConWriteChar);
    break;
  case prog::sym::FuncKind::ActionConWriteString:
    m_asmb->addPCall(novasm::PCallCode::ConWriteString);
    break;
  case prog::sym::FuncKind::ActionConWriteStringLine:
    m_asmb->addPCall(novasm::PCallCode::ConWriteStringLine);
    break;

  case prog::sym::FuncKind::ActionConReadChar:
    m_asmb->addPCall(novasm::PCallCode::ConReadChar);
    break;
  case prog::sym::FuncKind::ActionConReadStringLine:
    m_asmb->addPCall(novasm::PCallCode::ConReadStringLine);
    break;

  case prog::sym::FuncKind::ActionGetEnvArg:
    m_asmb->addPCall(novasm::PCallCode::GetEnvArg);
    break;
  case prog::sym::FuncKind::ActionGetEnvArgCount:
    m_asmb->addPCall(novasm::PCallCode::GetEnvArgCount);
    break;
  case prog::sym::FuncKind::ActionGetEnvVar:
    m_asmb->addPCall(novasm::PCallCode::GetEnvVar);
    break;

  case prog::sym::FuncKind::ActionClockMicroSinceEpoch:
    m_asmb->addPCall(novasm::PCallCode::ClockMicroSinceEpoch);
    break;
  case prog::sym::FuncKind::ActionClockNanoSteady:
    m_asmb->addPCall(novasm::PCallCode::ClockNanoSteady);
    break;

  case prog::sym::FuncKind::ActionSleepNano:
    m_asmb->addPCall(novasm::PCallCode::SleepNano);
    break;
  case prog::sym::FuncKind::ActionAssert:
    m_asmb->addPCall(novasm::PCallCode::Assert);
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
  m_asmb->addCallDyn(
      n.getChildCount() - 1,
      n.isFork() ? novasm::CallMode::Forked
                 : (m_tail ? novasm::CallMode::Tail : novasm::CallMode::Normal));
}

auto GenExpr::visit(const prog::expr::CallSelfExprNode& n) -> void {
  if (!m_curFunc) {
    throw std::logic_error{"Illegal self-call: Not inside a function"};
  }

  const auto& funcDecl = m_program.getFuncDecl(*m_curFunc);
  if (funcDecl.getOutput() != n.getType()) {
    throw std::logic_error{
        "Illegal self-call: Result-type does not match the type of the current function"};
  }

  const auto& funcDef        = m_program.getFuncDef(*m_curFunc);
  const auto& funcConstTable = funcDef.getConsts();

  const auto nonBoundInputs = funcConstTable.getNonBoundInputs();
  if (n.getChildCount() != nonBoundInputs.size()) {
    throw std::logic_error{
        "Illegal self-call: Number of arguments does not match non-bound argument "
        "count of current function"};
  }

  for (auto i = 0U; i != nonBoundInputs.size(); ++i) {
    if (n[i].getType() != funcConstTable[nonBoundInputs[i]].getType()) {
      throw std::logic_error{
          "Illegal self-call: Argument types do not match arguments of current function"};
    }
  }

  // Push the arguments on the stack.
  for (auto i = 0U; i < n.getChildCount(); ++i) {
    genSubExpr(n[i], false);
  }

  // Load the same bound arguments (if any) from the current function on the stack.
  for (const auto& boundInput : funcConstTable.getBoundInputs()) {
    m_asmb->addStackLoad(getConstOffset(m_constTable, boundInput));
  }

  // Invoke the current function.
  m_asmb->addCall(
      getLabel(funcDecl.getId()),
      funcDecl.getInput().getCount(),
      m_tail ? novasm::CallMode::Tail : novasm::CallMode::Normal);
}

auto GenExpr::visit(const prog::expr::ClosureNode& n) -> void {
  // Push the bound arguments on the stack.
  for (auto i = 0U; i < n.getChildCount(); ++i) {
    genSubExpr(n[i], false);
  }

  // Push the function instruction-pointer on the stack.
  const auto funcId = n.getFunc();
  m_asmb->addLoadLitIp(getLabel(funcId));

  // Create a struct containing both the bound arguments and the function pointer.
  m_asmb->addMakeStruct(n.getChildCount() + 1);
}

auto GenExpr::visit(const prog::expr::ConstExprNode& n) -> void {
  const auto constId = getConstOffset(m_constTable, n.getId());
  m_asmb->addStackLoad(constId);
}

auto GenExpr::visit(const prog::expr::FieldExprNode& n) -> void {
  // Load the struct.
  genSubExpr(n[0], false);

  const auto& structType = m_program.getTypeDecl(n[0].getType());
  if (structType.getKind() != prog::sym::TypeKind::Struct) {
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
  m_asmb->addLoadStructField(fieldId);
}

auto GenExpr::visit(const prog::expr::GroupExprNode& n) -> void {
  for (auto i = 0U; i < n.getChildCount(); ++i) {
    const auto last = i == n.getChildCount() - 1;
    genSubExpr(n[i], m_tail && last);

    // For all but the last expression we ignore the result.
    if (!last) {
      m_asmb->addPop();
    }
  }
}

auto GenExpr::visit(const prog::expr::UnionCheckExprNode& n) -> void {
  // Load the union.
  genSubExpr(n[0], false);

  // Test if the union is the correct type.
  m_asmb->addLoadStructField(0);
  m_asmb->addLoadLitInt(getUnionTypeId(m_program, n[0].getType(), n.getTargetType()));
  m_asmb->addCheckEqInt();
}

auto GenExpr::visit(const prog::expr::UnionGetExprNode& n) -> void {
  // Load the union.
  genSubExpr(n[0], false);

  // We need the union twice on the stack, once for the type check and once for getting the value.
  m_asmb->addDup();

  const auto typeEqLabel = m_asmb->generateLabel();
  const auto endLabel    = m_asmb->generateLabel();

  // Test if the union is the correct type.
  m_asmb->addLoadStructField(0);
  m_asmb->addLoadLitInt(getUnionTypeId(m_program, n[0].getType(), n.getTargetType()));
  m_asmb->addCheckEqInt();
  m_asmb->addJumpIf(typeEqLabel);

  m_asmb->addPop();         // Pop the extra union value from the stack (from the duplicate before).
  m_asmb->addLoadLitInt(0); // Load false.
  m_asmb->addJump(endLabel);

  m_asmb->label(typeEqLabel);

  // Store the union value as a const and load 'true' on the stack.
  const auto constId = getConstOffset(m_constTable, n.getConst());
  m_asmb->addLoadStructField(1);
  m_asmb->addStackStore(constId);

  m_asmb->addLoadLitInt(1); // Load true.

  m_asmb->label(endLabel);
}

auto GenExpr::visit(const prog::expr::FailNode & /*unused*/) -> void { m_asmb->addFail(); }

auto GenExpr::visit(const prog::expr::LitBoolNode& n) -> void {
  m_asmb->addLoadLitInt(n.getVal() ? 1U : 0U);
}

auto GenExpr::visit(const prog::expr::LitFloatNode& n) -> void {
  m_asmb->addLoadLitFloat(n.getVal());
}

auto GenExpr::visit(const prog::expr::LitFuncNode& n) -> void {
  const auto funcId = n.getFunc();
  m_asmb->addLoadLitIp(getLabel(funcId));
}

auto GenExpr::visit(const prog::expr::LitIntNode& n) -> void { m_asmb->addLoadLitInt(n.getVal()); }

auto GenExpr::visit(const prog::expr::LitLongNode& n) -> void {
  m_asmb->addLoadLitLong(n.getVal());
}

auto GenExpr::visit(const prog::expr::LitStringNode& n) -> void {
  m_asmb->addLoadLitString(n.getVal());
}

auto GenExpr::visit(const prog::expr::LitCharNode& n) -> void { m_asmb->addLoadLitInt(n.getVal()); }

auto GenExpr::visit(const prog::expr::LitEnumNode& n) -> void { m_asmb->addLoadLitInt(n.getVal()); }

auto GenExpr::genSubExpr(const prog::expr::Node& n, bool tail) -> void {
  auto genExpr = GenExpr{m_program, m_asmb, m_constTable, m_curFunc, tail};
  n.accept(&genExpr);
}

} // namespace backend::internal
