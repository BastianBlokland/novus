#include "prog/sym/action_decl.hpp"

namespace prog::sym {

ActionDecl::ActionDecl(ActionId id, ActionKind kind, std::string name, Input input) :
    m_id{id}, m_kind{kind}, m_name{std::move(name)}, m_input{std::move(input)} {}

auto ActionDecl::operator==(const ActionDecl& rhs) const noexcept -> bool {
  return m_id == rhs.m_id;
}

auto ActionDecl::operator!=(const ActionDecl& rhs) const noexcept -> bool {
  return !ActionDecl::operator==(rhs);
}

auto ActionDecl::getId() const -> const ActionId& { return m_id; }

auto ActionDecl::getKind() const -> const ActionKind& { return m_kind; }

auto ActionDecl::getName() const -> const std::string& { return m_name; }

auto ActionDecl::getInput() const -> const Input& { return m_input; }

auto operator<<(std::ostream& out, const ActionDecl& rhs) -> std::ostream& {
  return out << '[' << rhs.m_id << "]-'" << rhs.m_name << "'-" << rhs.m_input;
}

} // namespace prog::sym
