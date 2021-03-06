#include "novasm/disassembler.hpp"
#include "intrinsics.hpp"
#include "novasm/pcall_code.hpp"
#include <stdexcept>

namespace novasm {

namespace dasm {

InstructionArg::InstructionArg(int32_t value, std::vector<std::string> labels) :
    m_value{value}, m_labels{std::move(labels)} {}

InstructionArg::InstructionArg(int64_t value, std::vector<std::string> labels) :
    m_value{value}, m_labels{std::move(labels)} {}

InstructionArg::InstructionArg(uint32_t value, std::vector<std::string> labels) :
    m_value{value}, m_labels{std::move(labels)} {}

InstructionArg::InstructionArg(float value, std::vector<std::string> labels) :
    m_value{value}, m_labels{std::move(labels)} {}

InstructionArg::InstructionArg(PCallCode value, std::vector<std::string> labels) :
    m_value{value}, m_labels{std::move(labels)} {}

auto InstructionArg::getLabels() const noexcept -> const std::vector<std::string>& {
  return m_labels;
}

Instruction::Instruction(
    OpCode op,
    uint32_t ipOffset,
    std::vector<InstructionArg> args,
    std::vector<std::string> labels) :
    m_op{op}, m_ipOffset{ipOffset}, m_args{std::move(args)}, m_labels{std::move(labels)} {}

auto Instruction::getOp() const noexcept -> OpCode { return m_op; }

auto Instruction::getIpOffset() const noexcept -> uint32_t { return m_ipOffset; }

auto Instruction::getArgs() const noexcept -> const std::vector<InstructionArg>& { return m_args; }

auto Instruction::getLabels() const noexcept -> const std::vector<std::string>& { return m_labels; }

auto operator<<(std::ostream& out, const InstructionArg& rhs) -> std::ostream& {
  if (std::holds_alternative<int32_t>(rhs.m_value)) {
    out << std::get<int32_t>(rhs.m_value);
  } else if (std::holds_alternative<int64_t>(rhs.m_value)) {
    out << std::get<int64_t>(rhs.m_value);
  } else if (std::holds_alternative<uint32_t>(rhs.m_value)) {
    out << std::get<uint32_t>(rhs.m_value);
  } else if (std::holds_alternative<float>(rhs.m_value)) {
    out << std::get<float>(rhs.m_value);
  } else if (std::holds_alternative<PCallCode>(rhs.m_value)) {
    out << std::get<PCallCode>(rhs.m_value);
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
NO_SANITIZE(alignment)
inline auto readAsm(const uint8_t** ip) {
  const Type v = *reinterpret_cast<const Type*>(*ip); // NOLINT: Reinterpret cast
  *ip += sizeof(Type);
  return v;
}

static auto getLabels(const dasm::InstructionLabels& instrLabels, uint32_t ipOffset)
    -> std::vector<std::string> {

  const auto itr = instrLabels.find(ipOffset);
  if (itr == instrLabels.end()) {
    return {};
  }
  return itr->second;
}

auto disassembleInstructions(
    const Executable& executable, const dasm::InstructionLabels& instrLabels)
    -> std::vector<dasm::Instruction> {

  using Instr = dasm::Instruction;
  using Arg   = dasm::InstructionArg;

  auto result = std::vector<dasm::Instruction>{};

  for (auto* ip = executable.getIp(0); !executable.isEnd(ip);) {
    const auto offset = executable.getOffset(ip);
    const auto opCode = readAsm<OpCode>(&ip);
    const auto labels = getLabels(instrLabels, offset);

    switch (opCode) {
    case OpCode::LoadLitInt:
    case OpCode::MakeAtomic:
    case OpCode::AtomicBlock:
      result.push_back(Instr{opCode, offset, {Arg{readAsm<int32_t>(&ip)}}, labels});
      continue;
    case OpCode::LoadLitLong:
      result.push_back(Instr{opCode, offset, {Arg{readAsm<int64_t>(&ip)}}, labels});
      continue;
    case OpCode::LoadLitFloat:
      result.push_back(Instr{opCode, offset, {Arg{readAsm<float>(&ip)}}, labels});
      continue;
    case OpCode::StackAlloc:
    case OpCode::StackLoad:
    case OpCode::StackStore:
      result.push_back(Instr{opCode, offset, {Arg{readAsm<uint16_t>(&ip)}}, labels});
      continue;
    case OpCode::LoadLitIntSmall:
    case OpCode::StackAllocSmall:
    case OpCode::StackLoadSmall:
    case OpCode::StackStoreSmall:
    case OpCode::MakeStruct:
    case OpCode::StructLoadField:
    case OpCode::StructStoreField:
    case OpCode::CallDyn:
    case OpCode::CallDynTail:
    case OpCode::CallDynForked:
      result.push_back(Instr{opCode, offset, {Arg{readAsm<uint8_t>(&ip)}}, labels});
      continue;
    case OpCode::LoadLitInt0:
    case OpCode::LoadLitInt1:
    case OpCode::AddInt:
    case OpCode::AddLong:
    case OpCode::AddFloat:
    case OpCode::AddString:
    case OpCode::AppendChar:
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
    case OpCode::ShiftLeftLong:
    case OpCode::ShiftRightInt:
    case OpCode::ShiftRightLong:
    case OpCode::AndInt:
    case OpCode::AndLong:
    case OpCode::OrInt:
    case OpCode::OrLong:
    case OpCode::XorInt:
    case OpCode::XorLong:
    case OpCode::InvInt:
    case OpCode::InvLong:
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
    case OpCode::CheckStructNull:
    case OpCode::ConvIntLong:
    case OpCode::ConvIntFloat:
    case OpCode::ConvLongInt:
    case OpCode::ConvLongFloat:
    case OpCode::ConvFloatInt:
    case OpCode::ConvIntString:
    case OpCode::ConvLongString:
    case OpCode::ConvFloatString:
    case OpCode::ConvCharString:
    case OpCode::ConvIntChar:
    case OpCode::ConvLongChar:
    case OpCode::ConvFloatChar:
    case OpCode::ConvFloatLong:
    case OpCode::AtomicLoad:
    case OpCode::MakeNullStruct:
    case OpCode::Ret:
    case OpCode::Fail:
    case OpCode::FutureWaitNano:
    case OpCode::FutureBlock:
    case OpCode::Dup:
    case OpCode::Pop:
    case OpCode::Swap: {
      result.push_back(Instr{opCode, offset, {}, labels});
      continue;
    }
    case OpCode::LoadLitString:
      result.push_back(Instr{opCode, offset, {Arg{readAsm<uint32_t>(&ip)}}, labels});
      continue;
    case OpCode::AtomicCompareSwap: {
      const auto a = readAsm<int32_t>(&ip);
      const auto b = readAsm<int32_t>(&ip);
      result.push_back(Instr{opCode, offset, {Arg{a}, Arg{b}}, labels});
      continue;
    }
    case OpCode::LoadLitIp:
    case OpCode::Jump:
    case OpCode::JumpIf: {
      const auto tgtIpOffset = readAsm<uint32_t>(&ip);
      result.push_back(
          Instr{opCode, offset, {Arg{tgtIpOffset, getLabels(instrLabels, tgtIpOffset)}}, labels});
      continue;
    }
    case OpCode::Call:
    case OpCode::CallTail:
    case OpCode::CallForked: {
      const auto argC     = readAsm<uint8_t>(&ip);
      const auto ipOffset = readAsm<uint32_t>(&ip);
      result.push_back(Instr{
          opCode, offset, {Arg{argC}, Arg{ipOffset, getLabels(instrLabels, ipOffset)}}, labels});
      continue;
    }
    case OpCode::PCall:
      result.push_back(Instr{opCode, offset, {Arg{readAsm<PCallCode>(&ip)}}, labels});
      continue;
    }
    throw std::logic_error{"Bad assembly"};
  }
  return result;
}

} // namespace novasm
