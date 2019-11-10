#pragma once
#include "prog/sym/const_decl.hpp"
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace prog::sym {

class ConstDeclTable final {
public:
  using iterator = typename std::vector<ConstDecl>::const_iterator;

  ConstDeclTable()                              = default;
  ConstDeclTable(const ConstDeclTable& rhs)     = delete;
  ConstDeclTable(ConstDeclTable&& rhs) noexcept = default;
  ~ConstDeclTable()                             = default;

  auto operator=(const ConstDeclTable& rhs) -> ConstDeclTable& = delete;
  auto operator=(ConstDeclTable&& rhs) noexcept -> ConstDeclTable& = delete;

  [[nodiscard]] auto operator[](ConstId id) const -> const ConstDecl&;

  [[nodiscard]] auto begin() const -> iterator;
  [[nodiscard]] auto end() const -> iterator;

  [[nodiscard]] auto getAll() const -> std::vector<ConstId>;

  [[nodiscard]] auto lookup(const std::string& name) const -> std::optional<ConstId>;

  auto registerInput(std::string name, TypeId type) -> ConstId;
  auto registerLocal(std::string name, TypeId type) -> ConstId;

private:
  std::vector<ConstDecl> m_types;
  std::unordered_map<std::string, ConstId> m_lookup;

  auto registerConst(ConstKind kind, std::string name, TypeId type) -> ConstId;
};

} // namespace prog::sym
