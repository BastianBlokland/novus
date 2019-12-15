#include "prog/sym/func_decl.hpp"

namespace prog::sym {

FuncDecl::FuncDecl(FuncId id, FuncKind kind, std::string name, TypeSet input, TypeId output) :
    m_id{id}, m_kind{kind}, m_name{std::move(name)}, m_input{std::move(input)}, m_output{output} {}

auto FuncDecl::operator==(const FuncDecl& rhs) const noexcept -> bool { return m_id == rhs.m_id; }

auto FuncDecl::operator!=(const FuncDecl& rhs) const noexcept -> bool {
  return !FuncDecl::operator==(rhs);
}

auto FuncDecl::getId() const -> const FuncId& { return m_id; }

auto FuncDecl::getKind() const -> const FuncKind& { return m_kind; }

auto FuncDecl::getName() const -> const std::string& { return m_name; }

auto FuncDecl::getInput() const -> const TypeSet& { return m_input; }

auto FuncDecl::getOutput() const -> TypeId { return m_output; }

auto FuncDecl::updateOutput(TypeId newOutput) -> void { m_output = newOutput; }

auto operator<<(std::ostream& out, const FuncDecl& rhs) -> std::ostream& {
  return out << '[' << rhs.m_id << "]-'" << rhs.m_name << "'-" << '(' << rhs.m_input << ")->"
             << rhs.m_output;
}

} // namespace prog::sym
