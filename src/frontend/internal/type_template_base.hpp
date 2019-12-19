#pragma once
#include "internal/type_substitution_table.hpp"
#include "internal/type_template_inst.hpp"

namespace frontend::internal {

class Context;

class TypeTemplateBase {
public:
  TypeTemplateBase()                                = delete;
  TypeTemplateBase(const TypeTemplateBase& rhs)     = delete;
  TypeTemplateBase(TypeTemplateBase&& rhs) noexcept = default;
  ~TypeTemplateBase()                               = default;

  auto operator=(const TypeTemplateBase& rhs) -> TypeTemplateBase& = delete;
  auto operator=(TypeTemplateBase&& rhs) noexcept -> TypeTemplateBase& = delete;

  [[nodiscard]] auto getTemplateName() const -> const std::string&;
  [[nodiscard]] auto getTypeParamCount() const -> unsigned int;

  auto instantiate(const prog::sym::TypeSet& typeParams) -> const TypeTemplateInst*;

protected:
  TypeTemplateBase(Context* context, std::string name, std::vector<std::string> typeSubs);

  [[nodiscard]] auto getContext() const -> Context*;
  [[nodiscard]] auto getTypeSubs() const -> const std::vector<std::string>&;

  [[nodiscard]] auto createSubTable(const prog::sym::TypeSet& typeParams) const
      -> TypeSubstitutionTable;

private:
  Context* m_context;
  std::string m_name;
  std::vector<std::string> m_typeSubs;
  std::vector<std::unique_ptr<TypeTemplateInst>> m_instances;

  virtual auto setupInstance(TypeTemplateInst* instance) -> void = 0;
};

} // namespace frontend::internal
