#include "func_template_inst.hpp"

namespace frontend::internal {

FuncTemplateInst::FuncTemplateInst(prog::sym::TypeSet typeParams) :
    m_typeParams{std::move(typeParams)}, m_func{std::nullopt} {}

auto FuncTemplateInst::getTypeParams() const noexcept -> const prog::sym::TypeSet& {
  return m_typeParams;
}

auto FuncTemplateInst::hasErrors() const noexcept -> bool { return !m_diags.empty(); }

auto FuncTemplateInst::getFunc() const noexcept -> std::optional<prog::sym::FuncId> {
  return m_func;
}

auto FuncTemplateInst::getDiags() const noexcept -> const std::vector<Diag>& { return m_diags; }

} // namespace frontend::internal
