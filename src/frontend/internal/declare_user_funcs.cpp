#include "internal/declare_user_funcs.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"

namespace frontend::internal {

DeclareUserFuncs::DeclareUserFuncs(
    const Source& src, prog::Program* prog, FuncTemplateTable* funcTemplates) :
    m_src{src}, m_prog{prog}, m_funcTemplates(funcTemplates) {
  if (m_prog == nullptr) {
    throw std::invalid_argument{"Program cannot be null"};
  }
  if (m_funcTemplates == nullptr) {
    throw std::invalid_argument{"FuncTemplatesTable cannot be null"};
  }
}

auto DeclareUserFuncs::hasErrors() const noexcept -> bool { return !m_diags.empty(); }

auto DeclareUserFuncs::getDiags() const noexcept -> const std::vector<Diag>& { return m_diags; }

auto DeclareUserFuncs::getFuncs() const noexcept -> const std::vector<DeclarationInfo>& {
  return m_funcs;
}

auto DeclareUserFuncs::visit(const parse::FuncDeclStmtNode& n) -> void {
  std::string name;
  std::string displayName;
  if (n.getId().getKind() == lex::TokenKind::Identifier) {
    name = displayName = getName(n.getId());
    if (!validateFuncName(n.getId())) {
      return;
    }
  } else {
    auto op = getOperator(n.getId());
    if (!op) {
      m_diags.push_back(errNonOverloadableOperator(m_src, n.getId().str(), n.getId().getSpan()));
      return;
    }
    name        = prog::getFuncName(*op);
    displayName = "operator-" + n.getId().str();
  }

  // Check if this function is a conversion.
  const auto convType = m_prog->lookupType(name);
  if (convType) {
    if (n.getRetType()) {
      m_diags.push_back(errConvFuncCannotSpecifyReturnType(m_src, name, n.getId().getSpan()));
      return;
    }
  }

  // If the function is a template then we don't declare it in the program yet but declare it in
  // the function-template table.
  if (n.getTypeSubs()) {
    auto typeSubs = getSubstitutionParams(m_src, *m_prog, *n.getTypeSubs(), &m_diags);
    if (typeSubs) {
      m_funcTemplates->declare(name, std::move(*typeSubs), n);
    }
    return;
  }

  // Get func input.
  const auto input = getFuncInput(m_src, *m_prog, nullptr, n, &m_diags);
  if (!input) {
    return;
  }

  // Verify that this is not a duplicate declaration.
  if (m_prog->lookupFunc(name, input.value(), 0)) {
    m_diags.push_back(errDuplicateFuncDeclaration(m_src, displayName, n.getSpan()));
    return;
  }

  // Get return type.
  auto retType = convType ? convType : getRetType(m_src, *m_prog, nullptr, n, &m_diags);
  if (!retType) {
    return;
  }

  // Declare the function in the program.
  auto funcId = m_prog->declareUserFunc(name, input.value(), retType.value());
  m_funcs.emplace_back(funcId, n);
}

auto DeclareUserFuncs::validateFuncName(const lex::Token& nameToken) -> bool {
  const auto name = getName(nameToken);
  if (!m_prog->lookupActions(name).empty()) {
    m_diags.push_back(errFuncNameConflictsWithAction(m_src, name, nameToken.getSpan()));
    return false;
  }
  return true;
}

} // namespace frontend::internal
