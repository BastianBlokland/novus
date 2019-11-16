#include "prog/sym/func_decl.hpp"

namespace prog::sym {

FuncDecl::FuncDecl(FuncId id, FuncKind kind, std::string name, FuncSig sig) :
    m_id{id}, m_kind{kind}, m_name{std::move(name)}, m_sig{std::move(sig)} {}

auto FuncDecl::operator==(const FuncDecl& rhs) const noexcept -> bool { return m_id == rhs.m_id; }

auto FuncDecl::operator!=(const FuncDecl& rhs) const noexcept -> bool {
  return !FuncDecl::operator==(rhs);
}

auto FuncDecl::getId() const -> const FuncId& { return m_id; }

auto FuncDecl::getKind() const -> const FuncKind& { return m_kind; }

auto FuncDecl::getName() const -> const std::string& { return m_name; }

auto FuncDecl::getSig() const -> const FuncSig& { return m_sig; }

auto operator<<(std::ostream& out, const FuncDecl& rhs) -> std::ostream& {
  return out << '[' << rhs.m_id << "]-'" << rhs.m_name << "'-" << rhs.m_sig;
}

} // namespace prog::sym
