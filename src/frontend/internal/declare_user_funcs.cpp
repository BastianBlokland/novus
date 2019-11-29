#include "internal/declare_user_funcs.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"

namespace frontend::internal {

DeclareUserFuncs::DeclareUserFuncs(const Source& src, prog::Program* prog) :
    m_src{src}, m_prog{prog} {
  if (m_prog == nullptr) {
    throw std::invalid_argument{"Program cannot be null"};
  }
}

auto DeclareUserFuncs::hasErrors() const noexcept -> bool { return !m_diags.empty(); }

auto DeclareUserFuncs::getDiags() const noexcept -> const std::vector<Diag>& { return m_diags; }

auto DeclareUserFuncs::getFuncs() const noexcept -> const std::vector<DeclarationInfo>& {
  return m_funcs;
}

auto DeclareUserFuncs::visit(const parse::FuncDeclStmtNode& n) -> void {
  // Get func name.
  const auto name = getName(n.getId());
  if (!validateFuncName(n.getId())) {
    return;
  }

  // Get func input.
  const auto input = getFuncInput(n);
  if (!input) {
    return;
  }

  // Verify that this is not a duplicate declaration.
  if (m_prog->lookupFunc(name, input.value(), 0)) {
    m_diags.push_back(errDuplicateFuncDeclaration(m_src, name, n.getSpan()));
    return;
  }

  // Get return type.
  const auto retType = getRetType(n);
  if (!retType) {
    return;
  }

  // Declare the function in the program.
  auto funcId = m_prog->declareUserFunc(name, input.value(), retType.value());
  m_funcs.emplace_back(funcId, n);
}

auto DeclareUserFuncs::getRetType(const parse::FuncDeclStmtNode& n)
    -> std::optional<prog::sym::TypeId> {
  const auto& retTypeSpec = n.getRetType();
  if (!retTypeSpec) {
    return prog::sym::TypeId::inferType();
  }
  const auto retTypeName = getName(retTypeSpec->getType());
  auto retType           = m_prog->lookupType(retTypeName);
  if (!retType) {
    m_diags.push_back(errUndeclaredType(m_src, retTypeName, retTypeSpec->getType().getSpan()));
    return std::nullopt;
  }
  return retType;
}

auto DeclareUserFuncs::validateFuncName(const lex::Token& nameToken) -> bool {
  const auto name = getName(nameToken);
  if (m_prog->lookupType(name)) {
    m_diags.push_back(errFuncNameConflictsWithType(m_src, name, nameToken.getSpan()));
    return false;
  }
  if (!m_prog->lookupActions(name).empty()) {
    m_diags.push_back(errFuncNameConflictsWithAction(m_src, name, nameToken.getSpan()));
    return false;
  }
  return true;
}

auto DeclareUserFuncs::getFuncInput(const parse::FuncDeclStmtNode& n)
    -> std::optional<prog::sym::Input> {
  auto isValid  = true;
  auto argTypes = std::vector<prog::sym::TypeId>{};
  for (const auto& arg : n.getArgs()) {
    const auto argTypeName = getName(arg.getType());
    const auto argType     = m_prog->lookupType(argTypeName);
    if (argType) {
      argTypes.push_back(argType.value());
    } else {
      m_diags.push_back(errUndeclaredType(m_src, argTypeName, arg.getType().getSpan()));
      isValid = false;
    }
  }
  return isValid ? std::optional{prog::sym::Input{std::move(argTypes)}} : std::nullopt;
}

} // namespace frontend::internal
