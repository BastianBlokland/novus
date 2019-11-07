#include "internal/define_user_funcs.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/get_expr.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"

namespace frontend::internal {

DefineUserFuncs::DefineUserFuncs(const Source& src, prog::Program* prog) :
    m_src{src}, m_prog{prog} {}

auto DefineUserFuncs::hasErrors() const noexcept -> bool { return !m_diags.empty(); }

auto DefineUserFuncs::getDiags() const noexcept -> const std::vector<Diag>& { return m_diags; }

auto DefineUserFuncs::visit(const parse::FuncDeclStmtNode& n) -> void {
  const auto id = getFuncId(n);
  auto consts   = prog::sym::ConstDeclTable{};
  if (!declareInputs(n, &consts)) {
    return;
  }
  const auto& funcDecl = m_prog->getFuncDecl(id);

  auto visibleConsts = consts.getAll();
  auto getExpr       = GetExpr{m_src, m_prog, &consts, &visibleConsts};
  n[0].accept(&getExpr);
  m_diags.insert(m_diags.end(), getExpr.getDiags().begin(), getExpr.getDiags().end());
  if (!getExpr.getValue()) {
    return;
  }

  if (getExpr.getValue()->getType() != funcDecl.getSig().getOutput()) {
    const auto& declaredType = m_prog->getTypeDecl(funcDecl.getSig().getOutput()).getName();
    const auto& returnedType = m_prog->getTypeDecl(getExpr.getValue()->getType()).getName();
    m_diags.push_back(errNonMatchingFuncReturnType(
        m_src, funcDecl.getName(), declaredType, returnedType, n[0].getSpan()));
    return;
  }

  m_prog->defineUserFunc(id, std::move(consts), std::move(getExpr.getValue()));
}

auto DefineUserFuncs::getFuncId(const parse::FuncDeclStmtNode& n) -> prog::sym::FuncId {
  auto argTypes = std::vector<prog::sym::TypeId>{};
  for (const auto& arg : n.getArgs()) {
    const auto argType = m_prog->lookupType(getName(arg.first));
    if (argType) {
      argTypes.push_back(argType.value());
    }
  }
  const auto result = m_prog->lookupFunc(getName(n.getId()), prog::sym::Input{std::move(argTypes)});
  if (!result) {
    throw std::logic_error("Attempted to define function which was not declared yet");
  }
  return result.value();
}

auto DefineUserFuncs::declareInputs(
    const parse::FuncDeclStmtNode& n, prog::sym::ConstDeclTable* consts) -> bool {
  bool isValid = true;
  for (const auto& arg : n.getArgs()) {
    const auto name = getName(arg.second);
    if (m_prog->lookupType(name)) {
      m_diags.push_back(errConstNameConflictsWithType(m_src, name, arg.second.getSpan()));
      isValid = false;
      continue;
    }
    if (!m_prog->lookupFuncs(name).empty()) {
      m_diags.push_back(errConstNameConflictsWithFunction(m_src, name, arg.second.getSpan()));
      isValid = false;
      continue;
    }
    if (!m_prog->lookupActions(name).empty()) {
      m_diags.push_back(errConstNameConflictsWithAction(m_src, name, arg.second.getSpan()));
      isValid = false;
      continue;
    }
    if (consts->lookup(name)) {
      m_diags.push_back(errConstNameConflictsWithConst(m_src, name, arg.second.getSpan()));
      isValid = false;
      continue;
    }

    const auto argType = m_prog->lookupType(getName(arg.first));
    if (!argType) {
      // Fail because this should have been caught during function declaration.
      throw std::logic_error("No declaration found for function input");
    }
    consts->registerInput(getName(arg.second), argType.value());
  }
  return isValid;
}

} // namespace frontend::internal
