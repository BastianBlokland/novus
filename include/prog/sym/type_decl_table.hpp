#pragma once
#include "prog/sym/type_decl.hpp"
#include "prog/sym/type_id_hasher.hpp"
#include <optional>
#include <string>
#include <unordered_map>

namespace prog::sym {

// Type declaration table.
class TypeDeclTable final {
  constexpr static int reservedTypesCount = 1;

public:
  using Iterator = typename std::unordered_map<TypeId, TypeDecl, TypeIdHasher>::const_iterator;

  TypeDeclTable()                         = default;
  TypeDeclTable(const TypeDeclTable& rhs) = delete;
  TypeDeclTable(TypeDeclTable&& rhs)      = default;
  ~TypeDeclTable()                        = default;

  auto operator=(const TypeDeclTable& rhs) -> TypeDeclTable& = delete;
  auto operator=(TypeDeclTable&& rhs) noexcept -> TypeDeclTable& = default;

  [[nodiscard]] auto operator[](TypeId id) const -> const TypeDecl&;

  [[nodiscard]] auto getCount() const -> unsigned int;
  [[nodiscard]] auto begin() const -> Iterator;
  [[nodiscard]] auto end() const -> Iterator;

  [[nodiscard]] auto exists(const TypeId& id) const -> bool;
  [[nodiscard]] auto exists(const std::string& name) const -> bool;
  [[nodiscard]] auto lookup(const std::string& name) const -> std::optional<TypeId>;

  auto registerType(TypeKind kind, std::string name) -> TypeId;

  auto insertType(TypeId id, TypeKind kind, std::string name) -> void;

private:
  std::unordered_map<TypeId, TypeDecl, TypeIdHasher> m_types;
  std::unordered_map<std::string, TypeId> m_lookup;
  int m_highestIndex = reservedTypesCount - 1;
};

} // namespace prog::sym
