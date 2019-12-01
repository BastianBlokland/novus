#include "internal/declare_user_types.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"

namespace frontend::internal {

DeclareUserTypes::DeclareUserTypes(const Source& src, prog::Program* prog) :
    m_src{src}, m_prog{prog} {
  if (m_prog == nullptr) {
    throw std::invalid_argument{"Program cannot be null"};
  }
}

auto DeclareUserTypes::hasErrors() const noexcept -> bool { return !m_diags.empty(); }

auto DeclareUserTypes::getDiags() const noexcept -> const std::vector<Diag>& { return m_diags; }

auto DeclareUserTypes::getStructs() const noexcept -> const std::vector<StructDeclarationInfo>& {
  return m_structs;
}

auto DeclareUserTypes::visit(const parse::StructDeclStmtNode& n) -> void {
  // Get type name.
  const auto name = getName(n.getId());
  if (!validateTypeName(n.getId())) {
    return;
  }

  // Declare the struct in the program.
  auto typeId = m_prog->declareUserStruct(name);
  m_structs.emplace_back(typeId, n);
}

auto DeclareUserTypes::validateTypeName(const lex::Token& nameToken) -> bool {
  const auto name = getName(nameToken);
  if (m_prog->lookupType(name)) {
    m_diags.push_back(errTypeAlreadyDeclared(m_src, name, nameToken.getSpan()));
    return false;
  }
  if (!m_prog->lookupFuncs(name).empty()) {
    m_diags.push_back(errTypeNameConflictsWithFunc(m_src, name, nameToken.getSpan()));
    return false;
  }
  if (!m_prog->lookupActions(name).empty()) {
    m_diags.push_back(errTypeNameConflictsWithAction(m_src, name, nameToken.getSpan()));
    return false;
  }
  return true;
}

} // namespace frontend::internal
