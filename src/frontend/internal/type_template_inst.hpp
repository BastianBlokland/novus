#pragma once
#include "prog/program.hpp"

namespace frontend::internal {

class TypeTemplateInst final {
  friend class TypeTemplateBase;
  friend class StructTemplate;
  friend class UnionTemplate;

public:
  TypeTemplateInst() = delete;

  [[nodiscard]] auto getTypeParams() const noexcept -> const prog::sym::TypeSet& {
    return m_typeParams;
  }
  [[nodiscard]] auto getType() const noexcept -> std::optional<prog::sym::TypeId> { return m_type; }
  [[nodiscard]] auto isSuccess() const noexcept -> bool { return m_success; }

private:
  const prog::sym::TypeSet m_typeParams;
  std::optional<prog::sym::TypeId> m_type;
  bool m_success;

  explicit TypeTemplateInst(prog::sym::TypeSet typeParams) :
      m_typeParams{std::move(typeParams)}, m_type{std::nullopt}, m_success{true} {}
};

} // namespace frontend::internal
