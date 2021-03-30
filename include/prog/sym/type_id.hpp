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
  [[nodiscard]] static auto inferType() -> TypeId { return TypeId{0}; };

  auto operator==(const TypeId& rhs) const noexcept -> bool { return m_id == rhs.m_id; }
  auto operator!=(const TypeId& rhs) const noexcept -> bool { return !TypeId::operator==(rhs); }

  auto operator<(const TypeId& rhs) const noexcept -> bool { return m_id < rhs.m_id; }
  auto operator>(const TypeId& rhs) const noexcept -> bool { return m_id > rhs.m_id; }

  [[nodiscard]] auto getNum() const noexcept -> unsigned int { return m_id; }
  [[nodiscard]] auto isInfer() const noexcept -> bool { return m_id == 0; };
  [[nodiscard]] auto isConcrete() const noexcept -> bool { return m_id != 0; }

private:
  unsigned int m_id;

  explicit TypeId(unsigned int id) : m_id{id} {}
};

inline auto operator<<(std::ostream& out, const TypeId& rhs) -> std::ostream& {
  return out << "t-" << rhs.m_id;
}

} // namespace prog::sym
