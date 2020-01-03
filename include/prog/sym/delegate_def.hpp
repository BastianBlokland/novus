#pragma once
#include "prog/sym/type_id.hpp"
#include "prog/sym/type_set.hpp"

namespace prog::sym {

class DelegateDef final {
  friend class TypeDefTable;

public:
  DelegateDef()                           = delete;
  DelegateDef(const DelegateDef& rhs)     = delete;
  DelegateDef(DelegateDef&& rhs) noexcept = default;
  ~DelegateDef()                          = default;

  auto operator=(const DelegateDef& rhs) -> DelegateDef& = delete;
  auto operator=(DelegateDef&& rhs) noexcept -> DelegateDef& = delete;

  [[nodiscard]] auto getId() const noexcept -> const TypeId&;
  [[nodiscard]] auto getInput() const -> const TypeSet&;
  [[nodiscard]] auto getOutput() const -> TypeId;

private:
  sym::TypeId m_id;
  TypeSet m_input;
  TypeId m_output;

  DelegateDef(sym::TypeId id, TypeSet input, TypeId output);
};

} // namespace prog::sym
