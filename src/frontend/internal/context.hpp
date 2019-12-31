#pragma once
#include "frontend/diag.hpp"
#include "internal/delegate_table.hpp"
#include "internal/func_template_table.hpp"
#include "internal/type_info.hpp"
#include "internal/type_template_table.hpp"
#include "prog/program.hpp"
#include "prog/sym/func_id_hasher.hpp"
#include "prog/sym/type_id_hasher.hpp"
#include <unordered_map>

namespace frontend::internal {

class Context final {
public:
  Context() = delete;
  Context(
      const Source& src,
      prog::Program* prog,
      TypeTemplateTable* typeTemplates,
      FuncTemplateTable* funcTemplates,
      DelegateTable* delegates);

  [[nodiscard]] auto hasErrors() const noexcept -> bool;
  [[nodiscard]] auto getDiags() const noexcept -> const std::vector<Diag>&;

  [[nodiscard]] auto getSrc() const noexcept -> const Source&;
  [[nodiscard]] auto getProg() const noexcept -> prog::Program*;
  [[nodiscard]] auto getTypeTemplates() const noexcept -> TypeTemplateTable*;
  [[nodiscard]] auto getFuncTemplates() const noexcept -> FuncTemplateTable*;
  [[nodiscard]] auto getDelegates() const noexcept -> DelegateTable*;

  [[nodiscard]] auto getTypeInfo(prog::sym::TypeId typeId) const noexcept
      -> std::optional<TypeInfo>;

  auto declareTypeInfo(prog::sym::TypeId typeId, TypeInfo typeInfo) -> void;

  auto reportDiag(Diag diag) -> void;

private:
  const Source& m_src;
  prog::Program* m_prog;
  TypeTemplateTable* m_typeTemplates;
  FuncTemplateTable* m_funcTemplates;
  DelegateTable* m_delegates;
  std::vector<Diag> m_diags;

  std::unordered_map<prog::sym::TypeId, TypeInfo, prog::sym::TypeIdHasher> m_typeInfos;
};

} // namespace frontend::internal
