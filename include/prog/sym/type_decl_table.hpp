#pragma once
#include "prog/sym/type_decl.hpp"
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace prog::sym {

class TypeDeclTable final {
public:
  using iterator = typename std::vector<TypeDecl>::const_iterator;

  TypeDeclTable()                             = default;
  TypeDeclTable(const TypeDeclTable& rhs)     = delete;
  TypeDeclTable(TypeDeclTable&& rhs) noexcept = default;
  ~TypeDeclTable()                            = default;

  auto operator=(const TypeDeclTable& rhs) -> TypeDeclTable& = delete;
  auto operator=(TypeDeclTable&& rhs) noexcept -> TypeDeclTable& = delete;

  [[nodiscard]] auto operator[](TypeId id) const -> const TypeDecl&;

  [[nodiscard]] auto begin() const -> iterator;
  [[nodiscard]] auto end() const -> iterator;

  [[nodiscard]] auto exists(const std::string& name) const -> bool;
  [[nodiscard]] auto lookup(const std::string& name) const -> std::optional<TypeId>;

  auto registerType(TypeKind kind, std::string name) -> TypeId;

private:
  std::vector<TypeDecl> m_types;
  std::unordered_map<std::string, TypeId> m_lookup;
};

} // namespace prog::sym
