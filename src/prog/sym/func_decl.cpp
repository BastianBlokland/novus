#include "prog/sym/func_decl.hpp"

namespace prog::sym {

FuncDecl::FuncDecl(
    FuncId id,
    FuncKind kind,
    bool isAction,
    bool isIntrinsic,
    bool isImplicitConv,
    std::string name,
    TypeSet input,
    TypeId output,
    unsigned int numOptInputs) :
    m_id{id},
    m_kind{kind},
    m_isAction{isAction},
    m_isIntrinsic{isIntrinsic},
    m_isImplicitConv{isImplicitConv},
    m_name{std::move(name)},
    m_input{std::move(input)},
    m_output{output},
    m_numOptInputs{numOptInputs} {}

auto FuncDecl::operator==(const FuncDecl& rhs) const noexcept -> bool { return m_id == rhs.m_id; }

auto FuncDecl::operator!=(const FuncDecl& rhs) const noexcept -> bool {
  return !FuncDecl::operator==(rhs);
}

auto FuncDecl::getId() const noexcept -> const FuncId& { return m_id; }

auto FuncDecl::getKind() const noexcept -> const FuncKind& { return m_kind; }

auto FuncDecl::isAction() const noexcept -> bool { return m_isAction; }

auto FuncDecl::isIntrinsic() const noexcept -> bool { return m_isIntrinsic; }

auto FuncDecl::isImplicitConv() const noexcept -> bool { return m_isImplicitConv; }

auto FuncDecl::getName() const noexcept -> const std::string& { return m_name; }

auto FuncDecl::getInput() const noexcept -> const TypeSet& { return m_input; }

auto FuncDecl::getOutput() const noexcept -> TypeId { return m_output; }

auto FuncDecl::getNumOptInputs() const noexcept -> unsigned int { return m_numOptInputs; }

auto FuncDecl::getMinInputCount() const noexcept -> unsigned int {
  return m_input.getCount() - m_numOptInputs;
}

auto FuncDecl::updateOutput(TypeId newOutput) noexcept -> void { m_output = newOutput; }

auto operator<<(std::ostream& out, const FuncDecl& rhs) -> std::ostream& {
  return out << '[' << rhs.m_id << "]-'" << rhs.m_name << "'-" << '(' << rhs.m_input << ")->"
             << rhs.m_output;
}

} // namespace prog::sym
