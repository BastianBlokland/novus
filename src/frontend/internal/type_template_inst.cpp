#include "type_template_inst.hpp"

namespace frontend::internal {

TypeTemplateInst::TypeTemplateInst(prog::sym::TypeSet typeParams) :
    m_typeParams{std::move(typeParams)}, m_type{std::nullopt}, m_success{true} {}

auto TypeTemplateInst::getTypeParams() const noexcept -> const prog::sym::TypeSet& {
  return m_typeParams;
}

auto TypeTemplateInst::getType() const noexcept -> std::optional<prog::sym::TypeId> {
  return m_type;
}

auto TypeTemplateInst::isSuccess() const noexcept -> bool { return m_success; }

} // namespace frontend::internal
