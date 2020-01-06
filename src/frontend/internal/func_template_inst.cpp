#include "func_template_inst.hpp"
#include "internal/utilities.hpp"
#include <sstream>

namespace frontend::internal {

FuncTemplateInst::FuncTemplateInst(std::string baseName, prog::sym::TypeSet typeParams) :
    m_baseName{std::move(baseName)},
    m_typeParams{std::move(typeParams)},
    m_func{std::nullopt},
    m_retType{std::nullopt},
    m_success{true} {}

auto FuncTemplateInst::getDisplayName(const Context& context) const -> std::string {
  std::ostringstream oss;
  oss << m_baseName << '{';
  for (auto i = 0U; i < m_typeParams.getCount(); ++i) {
    if (i != 0) {
      oss << ',';
    }
    oss << internal::getDisplayName(context, m_typeParams[i]);
  }
  oss << '}';
  return oss.str();
}

auto FuncTemplateInst::getTypeParams() const noexcept -> const prog::sym::TypeSet& {
  return m_typeParams;
}

auto FuncTemplateInst::getFunc() const noexcept -> std::optional<prog::sym::FuncId> {
  return m_func;
}

auto FuncTemplateInst::getRetType() const noexcept -> std::optional<prog::sym::TypeId> {
  return m_retType;
}

auto FuncTemplateInst::isSuccess() const noexcept -> bool { return m_success; }

} // namespace frontend::internal
