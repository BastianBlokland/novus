#include "prog/sym/func_sig.hpp"

namespace prog::sym {

FuncSig::FuncSig(Input input, TypeId output) : m_input{std::move(input)}, m_output{output} {}

auto FuncSig::operator==(const FuncSig& rhs) const noexcept -> bool {
  return m_input == rhs.m_input && m_output == rhs.m_output;
}

auto FuncSig::operator!=(const FuncSig& rhs) const noexcept -> bool {
  return !FuncSig::operator==(rhs);
}

auto FuncSig::getInput() const -> const Input& { return m_input; }

auto FuncSig::getOutput() const -> const TypeId& { return m_output; }

auto operator<<(std::ostream& out, const FuncSig& rhs) -> std::ostream& {
  return out << '(' << rhs.m_input << ")->" << rhs.m_output;
}

} // namespace prog::sym
