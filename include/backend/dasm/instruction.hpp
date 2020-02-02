#pragma once
#include "vm/assembly.hpp"
#include "vm/op_code.hpp"
#include "vm/pcall_code.hpp"
#include <initializer_list>
#include <iostream>
#include <variant>

namespace backend::dasm {

using Arg = typename std::variant<int32_t, uint32_t, float, vm::PCallCode>;

class Instruction final {
  friend auto operator<<(std::ostream& out, const Instruction& rhs) -> std::ostream&;

public:
  Instruction(vm::OpCode op, uint32_t ipOffset, std::initializer_list<Arg> args);
  Instruction(vm::OpCode op, uint32_t ipOffset, std::vector<Arg> args);

  [[nodiscard]] auto getOp() const noexcept -> vm::OpCode;
  [[nodiscard]] auto getIpOffset() const noexcept -> uint32_t;
  [[nodiscard]] auto getArgs() const noexcept -> const std::vector<Arg>&;

private:
  vm::OpCode m_op;
  uint32_t m_ipOffset;
  std::vector<Arg> m_args;
};

auto operator<<(std::ostream& out, const Arg& rhs) -> std::ostream&;
auto operator<<(std::ostream& out, const Instruction& rhs) -> std::ostream&;

} // namespace backend::dasm
