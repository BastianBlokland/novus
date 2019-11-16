#include "assembly_output.hpp"
#include "backend/dasm/disassembler.hpp"

namespace vm {

auto operator<<(std::ostream& out, const vm::Assembly& rhs) -> std::ostream& {
  const auto instructions = backend::dasm::disassembleInstructions(rhs);
  for (auto i = 0U; i != instructions.size(); ++i) {
    if (i != 0) {
      out << '\n';
    }
    out << instructions[i];
  }
  return out;
}

} // namespace vm
