#pragma once
#include "vm/assembly.hpp"
#include "vm/opcode.hpp"
#include <initializer_list>
#include <iostream>

namespace backend::dasm {

class Instruction final {
  friend auto operator<<(std::ostream& out, const Instruction& rhs) -> std::ostream&;

public:
  Instruction(vm::OpCode op, uint32_t ipOffset, std::initializer_list<int64_t> args);
  Instruction(vm::OpCode op, uint32_t ipOffset, std::vector<int64_t> args);

  [[nodiscard]] auto getOp() const noexcept -> vm::OpCode;
  [[nodiscard]] auto getIpOffset() const noexcept -> uint32_t;
  [[nodiscard]] auto getArgs() const noexcept -> const std::vector<int64_t>&;

private:
  vm::OpCode m_op;
  uint32_t m_ipOffset;
  std::vector<int64_t> m_args;
};

auto operator<<(std::ostream& out, const Instruction& rhs) -> std::ostream&;

} // namespace backend::dasm
