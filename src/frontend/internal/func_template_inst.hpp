#pragma once
#include "frontend/diag.hpp"
#include "prog/program.hpp"

namespace frontend::internal {

class FuncTemplateInst final {
  friend class FuncTemplate;

public:
  FuncTemplateInst() = delete;

  [[nodiscard]] auto getTypeParams() const noexcept -> const prog::sym::TypeSet&;
  [[nodiscard]] auto hasErrors() const noexcept -> bool;

  [[nodiscard]] auto getFunc() const noexcept -> std::optional<prog::sym::FuncId>;
  [[nodiscard]] auto getRetType() const noexcept -> std::optional<prog::sym::TypeId>;
  [[nodiscard]] auto getDiags() const noexcept -> const std::vector<Diag>&;

private:
  const prog::sym::TypeSet m_typeParams;
  std::optional<prog::sym::FuncId> m_func;
  std::optional<prog::sym::TypeId> m_retType;
  std::vector<Diag> m_diags;

  explicit FuncTemplateInst(prog::sym::TypeSet typeParams);
};

} // namespace frontend::internal
