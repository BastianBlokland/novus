#include "novasm/disassembler.hpp"
#include "novasm/pcall_code.hpp"
#include <stdexcept>

namespace novasm {

namespace dasm {

Instruction::Instruction(OpCode op, uint32_t ipOffset, std::initializer_list<Arg> args) :
    m_op{op}, m_ipOffset{ipOffset}, m_args{args} {}

Instruction::Instruction(OpCode op, uint32_t ipOffset, std::vector<Arg> args) :
    m_op{op}, m_ipOffset{ipOffset}, m_args{std::move(args)} {}

auto Instruction::getOp() const noexcept -> OpCode { return m_op; }

auto Instruction::getIpOffset() const noexcept -> uint32_t { return m_ipOffset; }

auto Instruction::getArgs() const noexcept -> const std::vector<Arg>& { return m_args; }

auto operator<<(std::ostream& out, const Arg& rhs) -> std::ostream& {
  if (std::holds_alternative<int32_t>(rhs)) {
    out << std::get<int32_t>(rhs);
  } else if (std::holds_alternative<int64_t>(rhs)) {
    out << std::get<int64_t>(rhs);
  } else if (std::holds_alternative<uint32_t>(rhs)) {
    out << std::get<uint32_t>(rhs);
  } else if (std::holds_alternative<float>(rhs)) {
    out << std::get<float>(rhs);
  } else if (std::holds_alternative<PCallCode>(rhs)) {
    out << std::get<PCallCode>(rhs);
  } else {
    throw std::logic_error{"Unknown arg type"};
  }
  return out;
}

auto operator<<(std::ostream& out, const Instruction& rhs) -> std::ostream& {
  out << rhs.m_ipOffset << '-' << rhs.m_op;
  if (!rhs.m_args.empty()) {
    for (const auto& arg : rhs.m_args) {
      out << '-' << arg;
    }
  }
  return out;
}

} // namespace dasm

template <typename Type>
inline auto readAsm(const uint8_t** ip) {
  const Type v = *reinterpret_cast<const Type*>(*ip); // NOLINT: Reinterpret cast
  *ip += sizeof(Type);
  return v;
}

auto disassembleInstructions(const Assembly& assembly) -> std::vector<dasm::Instruction> {
  auto result = std::vector<dasm::Instruction>{};

  for (auto* ip = assembly.getIp(0); !assembly.isEnd(ip);) {
    const auto offset = assembly.getOffset(ip);
    const auto opCode = readAsm<OpCode>(&ip);
    switch (opCode) {
    case OpCode::LoadLitInt:
      result.push_back(dasm::Instruction(opCode, offset, {readAsm<int32_t>(&ip)}));
      continue;
    case OpCode::LoadLitLong:
      result.push_back(dasm::Instruction(opCode, offset, {readAsm<int64_t>(&ip)}));
      continue;
    case OpCode::LoadLitFloat:
      result.push_back(dasm::Instruction(opCode, offset, {readAsm<float>(&ip)}));
      continue;
    case OpCode::LoadLitIntSmall:
    case OpCode::StackAlloc:
    case OpCode::StackLoad:
    case OpCode::StackStore:
    case OpCode::MakeStruct:
    case OpCode::LoadStructField:
    case OpCode::CallDyn:
    case OpCode::CallDynTail:
    case OpCode::CallDynForked:
      result.push_back(dasm::Instruction(opCode, offset, {readAsm<uint8_t>(&ip)}));
      continue;
    case OpCode::LoadLitInt0:
    case OpCode::LoadLitInt1:
    case OpCode::AddInt:
    case OpCode::AddLong:
    case OpCode::AddFloat:
    case OpCode::AddString:
    case OpCode::SubInt:
    case OpCode::SubLong:
    case OpCode::SubFloat:
    case OpCode::MulInt:
    case OpCode::MulLong:
    case OpCode::MulFloat:
    case OpCode::DivInt:
    case OpCode::DivLong:
    case OpCode::DivFloat:
    case OpCode::RemInt:
    case OpCode::RemLong:
    case OpCode::ModFloat:
    case OpCode::PowFloat:
    case OpCode::SqrtFloat:
    case OpCode::SinFloat:
    case OpCode::CosFloat:
    case OpCode::TanFloat:
    case OpCode::ASinFloat:
    case OpCode::ACosFloat:
    case OpCode::ATanFloat:
    case OpCode::ATan2Float:
    case OpCode::NegInt:
    case OpCode::NegLong:
    case OpCode::NegFloat:
    case OpCode::LogicInvInt:
    case OpCode::ShiftLeftInt:
    case OpCode::ShiftRightInt:
    case OpCode::AndInt:
    case OpCode::OrInt:
    case OpCode::XorInt:
    case OpCode::InvInt:
    case OpCode::LengthString:
    case OpCode::IndexString:
    case OpCode::SliceString:
    case OpCode::CheckEqInt:
    case OpCode::CheckEqLong:
    case OpCode::CheckEqFloat:
    case OpCode::CheckEqString:
    case OpCode::CheckEqIp:
    case OpCode::CheckEqCallDynTgt:
    case OpCode::CheckGtInt:
    case OpCode::CheckGtLong:
    case OpCode::CheckGtFloat:
    case OpCode::CheckLeInt:
    case OpCode::CheckLeLong:
    case OpCode::CheckLeFloat:
    case OpCode::ConvIntLong:
    case OpCode::ConvIntFloat:
    case OpCode::ConvLongInt:
    case OpCode::ConvFloatInt:
    case OpCode::ConvIntString:
    case OpCode::ConvLongString:
    case OpCode::ConvFloatString:
    case OpCode::ConvCharString:
    case OpCode::ConvIntChar:
    case OpCode::Ret:
    case OpCode::Fail:
    case OpCode::FutureWaitNano:
    case OpCode::FutureBlock:
    case OpCode::Dup:
    case OpCode::Pop:
      result.push_back(dasm::Instruction(opCode, offset, {}));
      continue;
    case OpCode::LoadLitString:
    case OpCode::LoadLitIp:
    case OpCode::Jump:
    case OpCode::JumpIf:
      result.push_back(dasm::Instruction(opCode, offset, {readAsm<uint32_t>(&ip)}));
      continue;
    case OpCode::Call:
    case OpCode::CallTail:
    case OpCode::CallForked:
      result.push_back(
          dasm::Instruction(opCode, offset, {readAsm<uint8_t>(&ip), readAsm<uint32_t>(&ip)}));
      continue;
    case OpCode::PCall:
      result.push_back(dasm::Instruction(opCode, offset, {readAsm<PCallCode>(&ip)}));
      continue;
    }
    throw std::logic_error{"Bad assembly"};
  }
  return result;
}

} // namespace novasm
