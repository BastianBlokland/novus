#pragma once
#include "prog/sym/delegate_def.hpp"
#include "prog/sym/struct_def.hpp"
#include "prog/sym/type_decl_table.hpp"
#include "prog/sym/type_id_hasher.hpp"
#include "prog/sym/union_def.hpp"
#include <unordered_map>
#include <variant>

namespace prog::sym {

class TypeDefTable final {
public:
  using typeDef  = typename std::variant<StructDef, UnionDef, DelegateDef>;
  using iterator = typename std::unordered_map<TypeId, typeDef, TypeIdHasher>::const_iterator;

  TypeDefTable()                            = default;
  TypeDefTable(const TypeDefTable& rhs)     = delete;
  TypeDefTable(TypeDefTable&& rhs) noexcept = default;
  ~TypeDefTable()                           = default;

  auto operator=(const TypeDefTable& rhs) -> TypeDefTable& = delete;
  auto operator=(TypeDefTable&& rhs) noexcept -> TypeDefTable& = delete;

  [[nodiscard]] auto operator[](sym::TypeId id) const -> const typeDef&;

  [[nodiscard]] auto hasDef(sym::TypeId id) const -> bool;

  [[nodiscard]] auto begin() const -> iterator;
  [[nodiscard]] auto end() const -> iterator;

  auto
  registerStruct(const sym::TypeDeclTable& typeTable, sym::TypeId id, sym::FieldDeclTable fields)
      -> void;

  auto
  registerUnion(const sym::TypeDeclTable& typeTable, sym::TypeId id, std::vector<sym::TypeId> types)
      -> void;

  auto registerDelegate(
      const sym::TypeDeclTable& typeTable, sym::TypeId id, TypeSet input, TypeId output) -> void;

private:
  std::unordered_map<TypeId, typeDef, TypeIdHasher> m_types;
};

} // namespace prog::sym
