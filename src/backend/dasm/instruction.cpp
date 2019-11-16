#include "backend/dasm/instruction.hpp"

namespace backend::dasm {

Instruction::Instruction(vm::OpCode op, uint32_t ipOffset, std::initializer_list<int64_t> args) :
    m_op{op}, m_ipOffset{ipOffset}, m_args{args} {}

Instruction::Instruction(vm::OpCode op, uint32_t ipOffset, std::vector<int64_t> args) :
    m_op{op}, m_ipOffset{ipOffset}, m_args{std::move(args)} {}

auto Instruction::getOp() const noexcept -> vm::OpCode { return m_op; }

auto Instruction::getIpOffset() const noexcept -> uint32_t { return m_ipOffset; }

auto Instruction::getArgs() const noexcept -> const std::vector<int64_t>& { return m_args; }

auto operator<<(std::ostream& out, const Instruction& rhs) -> std::ostream& {
  out << rhs.m_ipOffset << '-' << rhs.m_op;
  if (!rhs.m_args.empty()) {
    for (const auto& arg : rhs.m_args) {
      out << '-' << arg;
    }
  }
  return out;
}

} // namespace backend::dasm
