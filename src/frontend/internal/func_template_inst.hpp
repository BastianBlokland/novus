#pragma once
#include "prog/program.hpp"

namespace frontend::internal {

class FuncTemplateInst final {
  friend class FuncTemplate;

public:
  FuncTemplateInst() = delete;

  [[nodiscard]] auto getTypeParams() const noexcept -> const prog::sym::TypeSet&;
  [[nodiscard]] auto getFunc() const noexcept -> std::optional<prog::sym::FuncId>;
  [[nodiscard]] auto getRetType() const noexcept -> std::optional<prog::sym::TypeId>;
  [[nodiscard]] auto isSuccess() const noexcept -> bool;

private:
  const prog::sym::TypeSet m_typeParams;
  std::optional<prog::sym::FuncId> m_func;
  std::optional<prog::sym::TypeId> m_retType;
  bool m_success;

  explicit FuncTemplateInst(prog::sym::TypeSet typeParams);
};

} // namespace frontend::internal
