#pragma once
#include "frontend/diag.hpp"
#include "internal/delegate_table.hpp"
#include "internal/fail_table.hpp"
#include "internal/func_template_table.hpp"
#include "internal/future_table.hpp"
#include "internal/lazy_table.hpp"
#include "internal/source_table_builder.hpp"
#include "internal/staticint_table.hpp"
#include "internal/type_info.hpp"
#include "internal/type_template_table.hpp"
#include "prog/program.hpp"
#include "prog/sym/func_id_hasher.hpp"
#include "prog/sym/type_id_hasher.hpp"
#include <unordered_map>

namespace frontend::internal {

class Context final {
  using TypeInfoMap = std::unordered_map<prog::sym::TypeId, TypeInfo, prog::sym::TypeIdHasher>;

public:
  Context() = delete;
  Context(
      const Source& src,
      prog::Program* prog,
      SourceTableBuilder* sourceTableBuilder,
      TypeTemplateTable* typeTemplates,
      FuncTemplateTable* funcTemplates,
      DelegateTable* delegates,
      FutureTable* futures,
      LazyTable* lazies,
      FailTable* fails,
      StaticIntTable* staticIntTable,
      TypeInfoMap* typeInfos,
      std::vector<Diag>* diags);

  [[nodiscard]] auto hasErrors() const noexcept -> bool;

  [[nodiscard]] auto getSrc() const noexcept -> const Source&;
  [[nodiscard]] auto getProg() const noexcept -> prog::Program*;
  [[nodiscard]] auto getTypeTemplates() const noexcept -> TypeTemplateTable*;
  [[nodiscard]] auto getFuncTemplates() const noexcept -> FuncTemplateTable*;
  [[nodiscard]] auto getDelegates() const noexcept -> DelegateTable*;
  [[nodiscard]] auto getFutures() const noexcept -> FutureTable*;
  [[nodiscard]] auto getLazies() const noexcept -> LazyTable*;
  [[nodiscard]] auto getFails() const noexcept -> FailTable*;
  [[nodiscard]] auto getStaticIntTable() const noexcept -> StaticIntTable*;

  [[nodiscard]] auto getTypeInfo(prog::sym::TypeId typeId) const noexcept
      -> std::optional<TypeInfo>;

  auto declareTypeInfo(prog::sym::TypeId typeId, TypeInfo typeInfo) -> void;

  template <typename DiagConstructor, class... Args>
  auto reportDiag(DiagConstructor constructor, input::Span span, Args&&... args) -> void {
    const auto srcId = m_sourceTableBuilder->add(&m_src, span);
    m_diags->push_back(constructor(srcId, std::forward<Args>(args)...));
  }

  auto associateSrc(const prog::expr::NodePtr& expr, input::Span span) -> void;

private:
  const Source& m_src;
  prog::Program* m_prog;
  SourceTableBuilder* m_sourceTableBuilder;
  TypeTemplateTable* m_typeTemplates;
  FuncTemplateTable* m_funcTemplates;
  DelegateTable* m_delegates;
  FutureTable* m_futures;
  LazyTable* m_lazies;
  FailTable* m_fails;
  StaticIntTable* m_staticIntTable;

  TypeInfoMap* m_typeInfos;
  std::vector<Diag>* m_diags;
};

} // namespace frontend::internal
