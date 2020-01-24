#pragma once
#include "prog/program.hpp"

namespace frontend::internal {

class Context;

class FuncTemplateInst final {
  friend class FuncTemplate;

public:
  FuncTemplateInst()                                = delete;
  FuncTemplateInst(const FuncTemplateInst& rhs)     = delete;
  FuncTemplateInst(FuncTemplateInst&& rhs) noexcept = default;
  ~FuncTemplateInst()                               = default;

  auto operator=(const FuncTemplateInst& rhs) -> FuncTemplateInst& = delete;
  auto operator=(FuncTemplateInst&& rhs) noexcept -> FuncTemplateInst& = default;

  [[nodiscard]] auto getDisplayName(const Context& ctx) const -> std::string;
  [[nodiscard]] auto getTypeParams() const noexcept -> const prog::sym::TypeSet&;
  [[nodiscard]] auto getFunc() const noexcept -> std::optional<prog::sym::FuncId>;
  [[nodiscard]] auto getRetType() const noexcept -> std::optional<prog::sym::TypeId>;
  [[nodiscard]] auto isSuccess() const noexcept -> bool;

private:
  std::string m_baseName;
  prog::sym::TypeSet m_typeParams;
  std::optional<prog::sym::FuncId> m_func;
  std::optional<prog::sym::TypeId> m_retType;
  bool m_success;

  explicit FuncTemplateInst(std::string baseName, prog::sym::TypeSet typeParams);
};

} // namespace frontend::internal
