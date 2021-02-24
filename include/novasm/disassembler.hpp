#pragma once
#include "novasm/executable.hpp"
#include "novasm/op_code.hpp"
#include "novasm/pcall_code.hpp"
#include <iostream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace novasm {

namespace dasm {

// Human readable strings to attach to instructions.
using InstructionLabels = std::unordered_map<uint32_t, std::vector<std::string>>;

class InstructionArg final {
  friend auto operator<<(std::ostream& out, const InstructionArg& rhs) -> std::ostream&;

public:
  explicit InstructionArg(int32_t value, std::vector<std::string> labels = {});
  explicit InstructionArg(int64_t value, std::vector<std::string> labels = {});
  explicit InstructionArg(uint32_t value, std::vector<std::string> labels = {});
  explicit InstructionArg(float value, std::vector<std::string> labels = {});
  explicit InstructionArg(PCallCode value, std::vector<std::string> labels = {});

  [[nodiscard]] auto getLabels() const noexcept -> const std::vector<std::string>&;

private:
  std::variant<int32_t, int64_t, uint32_t, float, PCallCode> m_value;
  std::vector<std::string> m_labels;
};

// Representation of a disassembled assembly instruction and its arguments.
class Instruction final {
  friend auto operator<<(std::ostream& out, const Instruction& rhs) -> std::ostream&;

public:
  Instruction(
      OpCode op,
      uint32_t ipOffset,
      std::vector<InstructionArg> args,
      std::vector<std::string> labels = {});

  [[nodiscard]] auto getOp() const noexcept -> OpCode;
  [[nodiscard]] auto getIpOffset() const noexcept -> uint32_t;
  [[nodiscard]] auto getArgs() const noexcept -> const std::vector<InstructionArg>&;
  [[nodiscard]] auto getLabels() const noexcept -> const std::vector<std::string>&;

private:
  OpCode m_op;
  uint32_t m_ipOffset;
  std::vector<InstructionArg> m_args;
  std::vector<std::string> m_labels;
};

auto operator<<(std::ostream& out, const InstructionArg& rhs) -> std::ostream&;
auto operator<<(std::ostream& out, const Instruction& rhs) -> std::ostream&;

} // namespace dasm

// Extract a list of instructions and their arguments from a executable.
// Note: Optionally instruction labels (as created by the assembler) can be provided to add human
// readable labels to the instructions.
auto disassembleInstructions(
    const Executable& executable, const dasm::InstructionLabels& instrLabels = {})
    -> std::vector<dasm::Instruction>;

} // namespace novasm
