#pragma once
#include <iostream>

namespace prog::sym {

// Identifier for a type, wrapper around an unsigned int.
//
// TypeId zero is a special sentinel to indicate the type still needs to be inferred.
// Note: Infer type should only be used internally during the syntax analysis, a final program
// should never contain a 'Infer' type.
class TypeId final {
  friend class TypeDeclTable;
  friend class TypeDefTable;
  friend class TypeIdHasher;
  friend auto operator<<(std::ostream& out, const TypeId& rhs) -> std::ostream&;

public:
  [[nodiscard]] static auto inferType() -> TypeId;

  auto operator==(const TypeId& rhs) const noexcept -> bool;
  auto operator!=(const TypeId& rhs) const noexcept -> bool;

  auto operator<(const TypeId& rhs) const noexcept -> bool;
  auto operator>(const TypeId& rhs) const noexcept -> bool;

  [[nodiscard]] auto getNum() const noexcept -> unsigned int;
  [[nodiscard]] auto isInfer() const noexcept -> bool;
  [[nodiscard]] auto isConcrete() const noexcept -> bool;

private:
  unsigned int m_id;

  explicit TypeId(unsigned int id);
};

auto operator<<(std::ostream& out, const TypeId& rhs) -> std::ostream&;

} // namespace prog::sym
