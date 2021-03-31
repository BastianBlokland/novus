#pragma once
#include "prog/sym/const_decl.hpp"
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace prog::sym {

// Constants declaration table.
//
// There are 3 kinds of constants:
// * Bound: Used in closures for constants that are passed from the parent scope.
// * Input: Used for function inputs.
// * Local: Normal constants used in a function body for example.
//
// To make the backends job easier these are kept in a well defined order:
// 1: Input
// 2: Bound
// 3: Local
//
class ConstDeclTable final {
public:
  using Iterator = std::vector<ConstDecl>::const_iterator;

  ConstDeclTable()                              = default;
  ConstDeclTable(const ConstDeclTable& rhs)     = default;
  ConstDeclTable(ConstDeclTable&& rhs) noexcept = default;
  ~ConstDeclTable()                             = default;

  auto operator=(const ConstDeclTable& rhs) -> ConstDeclTable& = delete;
  auto operator=(ConstDeclTable&& rhs) noexcept -> ConstDeclTable& = delete;

  [[nodiscard]] auto operator[](ConstId id) const -> const ConstDecl&;

  [[nodiscard]] auto getCount() const -> unsigned int;
  [[nodiscard]] auto getLocalCount() const -> unsigned int;

  [[nodiscard]] auto begin() const -> Iterator;
  [[nodiscard]] auto begin(ConstKind kind) const -> Iterator;
  [[nodiscard]] auto end() const -> Iterator;
  [[nodiscard]] auto end(ConstKind kind) const -> Iterator;

  [[nodiscard]] auto getAll() const -> std::vector<ConstId>;
  [[nodiscard]] auto getInputs() const -> std::vector<ConstId>;
  [[nodiscard]] auto getBoundInputs() const -> std::vector<ConstId>;
  [[nodiscard]] auto getNonBoundInputs() const -> std::vector<ConstId>;

  [[nodiscard]] auto lookup(const std::string& name) const -> std::optional<ConstId>;

  [[nodiscard]] auto getOffset(ConstId id) const -> unsigned int;
  [[nodiscard]] auto getHighestConstId() const -> ConstId;
  [[nodiscard]] auto getNextConstId() const noexcept -> ConstId {
    return ConstId{static_cast<unsigned int>(m_highestId + 1)};
  }

  auto registerBound(TypeId type) -> ConstId;
  auto registerInput(std::string name, TypeId type) -> ConstId;
  auto registerLocal(std::string name, TypeId type) -> ConstId;

  // Remove a constant from the table, note: care must be taken to avoid removing a constant that is
  // still being referenced.
  auto erase(ConstId id) -> bool;

private:
  std::vector<ConstDecl> m_consts;
  std::unordered_map<std::string, ConstId> m_lookup;
  int m_highestId = -1;

  auto registerConst(ConstKind kind, std::string name, TypeId type) -> ConstId;

  [[nodiscard]] auto getConstIds(ConstKind beginKind, ConstKind endKind) const
      -> std::vector<ConstId>;
};

} // namespace prog::sym
