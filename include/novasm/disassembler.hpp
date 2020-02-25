#pragma once
#include "novasm/assembly.hpp"
#include "novasm/op_code.hpp"
#include "novasm/pcall_code.hpp"
#include <initializer_list>
#include <iostream>
#include <variant>
#include <vector>

namespace novasm {

namespace dasm {

using Arg = typename std::variant<int32_t, int64_t, uint32_t, float, PCallCode>;

class Instruction final {
  friend auto operator<<(std::ostream& out, const Instruction& rhs) -> std::ostream&;

public:
  Instruction(OpCode op, uint32_t ipOffset, std::initializer_list<Arg> args);
  Instruction(OpCode op, uint32_t ipOffset, std::vector<Arg> args);

  [[nodiscard]] auto getOp() const noexcept -> OpCode;
  [[nodiscard]] auto getIpOffset() const noexcept -> uint32_t;
  [[nodiscard]] auto getArgs() const noexcept -> const std::vector<Arg>&;

private:
  OpCode m_op;
  uint32_t m_ipOffset;
  std::vector<Arg> m_args;
};

auto operator<<(std::ostream& out, const Arg& rhs) -> std::ostream&;
auto operator<<(std::ostream& out, const Instruction& rhs) -> std::ostream&;

} // namespace dasm

auto disassembleInstructions(const Assembly& assembly) -> std::vector<dasm::Instruction>;

} // namespace novasm
