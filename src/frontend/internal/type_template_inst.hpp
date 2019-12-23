#pragma once
#include "prog/program.hpp"

namespace frontend::internal {

class TypeTemplateInst final {
  friend class TypeTemplateBase;
  friend class StructTemplate;
  friend class UnionTemplate;

public:
  TypeTemplateInst() = delete;

  [[nodiscard]] auto getTypeParams() const noexcept -> const prog::sym::TypeSet&;
  [[nodiscard]] auto getType() const noexcept -> std::optional<prog::sym::TypeId>;

private:
  const prog::sym::TypeSet m_typeParams;
  std::optional<prog::sym::TypeId> m_type;

  explicit TypeTemplateInst(prog::sym::TypeSet typeParams);
};

} // namespace frontend::internal
