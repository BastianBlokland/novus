#include "type_template_base.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/utilities.hpp"

namespace frontend::internal {

TypeTemplateBase::TypeTemplateBase(
    Context* ctx, std::string name, std::vector<std::string> typeSubs) :
    m_ctx{ctx}, m_name{std::move(name)}, m_typeSubs{std::move(typeSubs)} {
  if (m_ctx == nullptr) {
    throw std::invalid_argument{"Context cannot be null"};
  }
}

auto TypeTemplateBase::getTemplateName() const -> const std::string& { return m_name; }

auto TypeTemplateBase::getTypeParamCount() const -> unsigned int { return m_typeSubs.size(); }

auto TypeTemplateBase::getCtx() const -> Context* { return m_ctx; }

auto TypeTemplateBase::getTypeSubs() const -> const std::vector<std::string>& { return m_typeSubs; }

auto TypeTemplateBase::instantiate(const prog::sym::TypeSet& typeParams)
    -> const TypeTemplateInst* {
  if (typeParams.getCount() != m_typeSubs.size()) {
    throw std::invalid_argument{"Invalid number of type-parameters provided"};
  }

  // Check if we have a previous instantiation for the given type params.
  const auto previousInst =
      std::find_if(m_instances.begin(), m_instances.end(), [&typeParams](const auto& inst) {
        return inst->getTypeParams() == typeParams;
      });
  if (previousInst != m_instances.end()) {
    return previousInst->get();
  }

  m_instances.push_back(std::make_unique<TypeTemplateInst>(TypeTemplateInst{typeParams}));
  auto instance = m_instances.back().get();

  setupInstance(instance);
  return instance;
}

auto TypeTemplateBase::createSubTable(const prog::sym::TypeSet& typeParams) const
    -> TypeSubstitutionTable {
  auto subTable = TypeSubstitutionTable{};
  for (auto i = 0U; i != m_typeSubs.size(); ++i) {
    subTable.declare(m_typeSubs[i], typeParams[i]);
  }
  return subTable;
}

} // namespace frontend::internal
