#include "assembly_output.hpp"
#include "novasm/disassembler.hpp"

namespace novasm {

auto operator<<(std::ostream& out, const novasm::Executable& rhs) -> std::ostream& {
  const auto instructions = novasm::disassembleInstructions(rhs);
  for (auto i = 0U; i != instructions.size(); ++i) {
    if (i != 0) {
      out << '\n';
    }
    out << instructions[i];
  }
  return out;
}

} // namespace novasm
