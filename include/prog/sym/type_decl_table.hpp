#pragma once
#include "prog/sym/type_decl.hpp"
#include <map>
#include <optional>
#include <string>
#include <unordered_map>

namespace prog::sym {

class TypeDeclTable final {
public:
  using Iterator = typename std::map<TypeId, TypeDecl>::const_iterator;

  TypeDeclTable()                             = default;
  TypeDeclTable(const TypeDeclTable& rhs)     = delete;
  TypeDeclTable(TypeDeclTable&& rhs) noexcept = default;
  ~TypeDeclTable()                            = default;

  auto operator=(const TypeDeclTable& rhs) -> TypeDeclTable& = delete;
  auto operator=(TypeDeclTable&& rhs) noexcept -> TypeDeclTable& = delete;

  [[nodiscard]] auto operator[](TypeId id) const -> const TypeDecl&;

  [[nodiscard]] auto begin() const -> Iterator;
  [[nodiscard]] auto end() const -> Iterator;

  [[nodiscard]] auto exists(const TypeId& id) const -> bool;
  [[nodiscard]] auto exists(const std::string& name) const -> bool;
  [[nodiscard]] auto lookup(const std::string& name) const -> std::optional<TypeId>;

  auto registerType(TypeKind kind, std::string name) -> TypeId;

  auto insertType(TypeId id, TypeKind kind, std::string name) -> void;

private:
  std::map<TypeId, TypeDecl> m_types;
  std::unordered_map<std::string, TypeId> m_lookup;
};

} // namespace prog::sym
