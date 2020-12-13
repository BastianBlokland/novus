#include "internal/context.hpp"
#include <sstream>

namespace frontend::internal {

Context::Context(
    const Source& src,
    prog::Program* prog,
    TypeTemplateTable* typeTemplates,
    FuncTemplateTable* funcTemplates,
    DelegateTable* delegates,
    FutureTable* futures,
    LazyTable* lazies,
    FailTable* fails,
    TypeInfoMap* typeInfos,
    std::vector<Diag>* diags) :
    m_src{src},
    m_prog{prog},
    m_typeTemplates{typeTemplates},
    m_funcTemplates{funcTemplates},
    m_delegates{delegates},
    m_futures{futures},
    m_lazies{lazies},
    m_fails{fails},
    m_typeInfos{typeInfos},
    m_diags{diags} {

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
  if (m_futures == nullptr) {
    throw std::invalid_argument{"FutureTable cannot be null"};
  }
  if (m_lazies == nullptr) {
    throw std::invalid_argument{"LazyTable cannot be null"};
  }
  if (m_fails == nullptr) {
    throw std::invalid_argument{"FailTable cannot be null"};
  }
  if (m_typeInfos == nullptr) {
    throw std::invalid_argument{"TypeInfoMap cannot be null"};
  }
  if (m_diags == nullptr) {
    throw std::invalid_argument{"Diagnostics vector cannot be null"};
  }
}

auto Context::hasErrors() const noexcept -> bool { return !m_diags->empty(); }

auto Context::getSrc() const noexcept -> const Source& { return m_src; }

auto Context::getProg() const noexcept -> prog::Program* { return m_prog; }

auto Context::getTypeTemplates() const noexcept -> TypeTemplateTable* { return m_typeTemplates; }

auto Context::getFuncTemplates() const noexcept -> FuncTemplateTable* { return m_funcTemplates; }

auto Context::getDelegates() const noexcept -> DelegateTable* { return m_delegates; }

auto Context::getFutures() const noexcept -> FutureTable* { return m_futures; }

auto Context::getLazies() const noexcept -> LazyTable* { return m_lazies; }

auto Context::getFails() const noexcept -> FailTable* { return m_fails; }

auto Context::getTypeInfo(prog::sym::TypeId typeId) const noexcept -> std::optional<TypeInfo> {
  const auto itr = m_typeInfos->find(typeId);
  if (itr == m_typeInfos->end()) {
    return std::nullopt;
  }
  return itr->second;
}

auto Context::declareTypeInfo(prog::sym::TypeId typeId, TypeInfo typeInfo) -> void {
  m_typeInfos->insert({typeId, std::move(typeInfo)});
}

} // namespace frontend::internal
