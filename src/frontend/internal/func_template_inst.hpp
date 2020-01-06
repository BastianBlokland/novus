#pragma once
#include "prog/program.hpp"

namespace frontend::internal {

class Context;

class FuncTemplateInst final {
  friend class FuncTemplate;

public:
  FuncTemplateInst() = delete;

  [[nodiscard]] auto getDisplayName(const Context& context) const -> std::string;
  [[nodiscard]] auto getTypeParams() const noexcept -> const prog::sym::TypeSet&;
  [[nodiscard]] auto getFunc() const noexcept -> std::optional<prog::sym::FuncId>;
  [[nodiscard]] auto getRetType() const noexcept -> std::optional<prog::sym::TypeId>;
  [[nodiscard]] auto isSuccess() const noexcept -> bool;

private:
  const std::string m_baseName;
  const prog::sym::TypeSet m_typeParams;
  std::optional<prog::sym::FuncId> m_func;
  std::optional<prog::sym::TypeId> m_retType;
  bool m_success;

  explicit FuncTemplateInst(std::string baseName, prog::sym::TypeSet typeParams);
};

} // namespace frontend::internal
