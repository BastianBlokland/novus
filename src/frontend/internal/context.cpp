#include "internal/context.hpp"

namespace frontend::internal {

Context::Context(
    const Source& src,
    prog::Program* prog,
    TypeTemplateTable* typeTemplates,
    FuncTemplateTable* funcTemplates,
    DelegateTable* delegates) :
    m_src{src},
    m_prog{prog},
    m_typeTemplates{typeTemplates},
    m_funcTemplates{funcTemplates},
    m_delegates(delegates) {

  if (m_prog == nullptr) {
    throw std::invalid_argument{"Program cannot be null"};
  }
  if (m_typeTemplates == nullptr) {
    throw std::invalid_argument{"TypeTemplatesTable cannot be null"};
  }
  if (m_funcTemplates == nullptr) {
    throw std::invalid_argument{"FuncTemplatesTable cannot be null"};
  }
  if (m_delegates == nullptr) {
    throw std::invalid_argument{"DelegateTable cannot be null"};
  }
}

auto Context::hasErrors() const noexcept -> bool { return !m_diags.empty(); }

auto Context::getDiags() const noexcept -> const std::vector<Diag>& { return m_diags; }

auto Context::getSrc() const noexcept -> const Source& { return m_src; }

auto Context::getProg() const noexcept -> prog::Program* { return m_prog; }

auto Context::getTypeTemplates() const noexcept -> TypeTemplateTable* { return m_typeTemplates; }

auto Context::getFuncTemplates() const noexcept -> FuncTemplateTable* { return m_funcTemplates; }

auto Context::getDelegates() const noexcept -> DelegateTable* { return m_delegates; }

auto Context::getTypeInfo(prog::sym::TypeId typeId) const noexcept -> std::optional<TypeInfo> {
  const auto itr = m_typeInfos.find(typeId);
  if (itr == m_typeInfos.end()) {
    return std::nullopt;
  }
  return itr->second;
}

auto Context::declareTypeInfo(prog::sym::TypeId typeId, TypeInfo typeInfo) -> void {
  m_typeInfos.insert({typeId, std::move(typeInfo)});
}

auto Context::reportDiag(Diag diag) -> void { m_diags.push_back(std::move(diag)); }

} // namespace frontend::internal
