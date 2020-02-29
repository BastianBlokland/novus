#pragma once
#include "prog/sym/delegate_def.hpp"
#include "prog/sym/enum_def.hpp"
#include "prog/sym/future_def.hpp"
#include "prog/sym/struct_def.hpp"
#include "prog/sym/type_decl_table.hpp"
#include "prog/sym/type_id_hasher.hpp"
#include "prog/sym/union_def.hpp"
#include <unordered_map>
#include <variant>

namespace prog::sym {

class TypeDefTable final {
public:
  using TypeDef  = typename std::variant<StructDef, UnionDef, EnumDef, DelegateDef, FutureDef>;
  using Iterator = typename std::unordered_map<TypeId, TypeDef, TypeIdHasher>::const_iterator;

  TypeDefTable()                            = default;
  TypeDefTable(const TypeDefTable& rhs)     = delete;
  TypeDefTable(TypeDefTable&& rhs) noexcept = default;
  ~TypeDefTable()                           = default;

  auto operator=(const TypeDefTable& rhs) -> TypeDefTable& = delete;
  auto operator=(TypeDefTable&& rhs) noexcept -> TypeDefTable& = delete;

  [[nodiscard]] auto operator[](sym::TypeId id) const -> const TypeDef&;

  [[nodiscard]] auto hasDef(sym::TypeId id) const -> bool;

  [[nodiscard]] auto begin() const -> Iterator;
  [[nodiscard]] auto end() const -> Iterator;

  auto
  registerStruct(const sym::TypeDeclTable& typeTable, sym::TypeId id, sym::FieldDeclTable fields)
      -> void;

  auto
  registerUnion(const sym::TypeDeclTable& typeTable, sym::TypeId id, std::vector<sym::TypeId> types)
      -> void;

  auto registerEnum(
      const sym::TypeDeclTable& typeTable,
      sym::TypeId id,
      std::unordered_map<std::string, int32_t> entries) -> void;

  auto registerDelegate(
      const sym::TypeDeclTable& typeTable,
      sym::TypeId id,
      bool isAction,
      TypeSet input,
      TypeId output) -> void;

  auto registerFuture(const sym::TypeDeclTable& typeTable, sym::TypeId id, TypeId result) -> void;

private:
  std::unordered_map<TypeId, TypeDef, TypeIdHasher> m_types;
};

} // namespace prog::sym
