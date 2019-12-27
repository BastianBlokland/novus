#pragma once
#include "internal/struct_template.hpp"
#include "internal/type_template_inst.hpp"
#include "internal/union_template.hpp"
#include <unordered_map>

namespace frontend::internal {

class Context;

class TypeTemplateTable final {
public:
  TypeTemplateTable()                                 = default;
  TypeTemplateTable(const TypeTemplateTable& rhs)     = delete;
  TypeTemplateTable(TypeTemplateTable&& rhs) noexcept = default;
  ~TypeTemplateTable()                                = default;

  auto operator=(const TypeTemplateTable& rhs) -> TypeTemplateTable& = delete;
  auto operator=(TypeTemplateTable&& rhs) noexcept -> TypeTemplateTable& = delete;

  auto declareStruct(
      Context* context,
      const std::string& name,
      std::vector<std::string> typeSubs,
      const parse::StructDeclStmtNode& n) -> void;

  auto declareUnion(
      Context* context,
      const std::string& name,
      std::vector<std::string> typeSubs,
      const parse::UnionDeclStmtNode& n) -> void;

  [[nodiscard]] auto hasType(const std::string& name) const -> bool;

  [[nodiscard]] auto instantiate(const std::string& name, const prog::sym::TypeSet& typeParams)
      -> std::optional<const TypeTemplateInst*>;

  [[nodiscard]] auto
  inferParamsAndInstantiate(const std::string& name, const prog::sym::TypeSet& constructorArgTypes)
      -> std::optional<const TypeTemplateInst*>;

private:
  std::unordered_map<std::string, StructTemplate> m_structs;
  std::unordered_map<std::string, UnionTemplate> m_unions;

  template <typename T>
  auto declareTemplate(
      std::unordered_map<std::string, T>* templates, const std::string& name, T newTemplate)
      -> void;

  template <typename T>
  auto instantiateTemplate(
      std::unordered_map<std::string, T>* templates,
      const std::string& name,
      const prog::sym::TypeSet& typeParams) const -> std::optional<const TypeTemplateInst*>;

  template <typename T>
  auto inferParamsAndInstantiate(
      std::unordered_map<std::string, T>* templates,
      const std::string& name,
      const prog::sym::TypeSet& constructorArgTypes) const
      -> std::optional<const TypeTemplateInst*>;
};

} // namespace frontend::internal
