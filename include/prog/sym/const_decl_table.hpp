#pragma once
#include "prog/sym/const_decl.hpp"
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace prog::sym {

class ConstDeclTable final {
public:
  using Iterator = typename std::vector<ConstDecl>::const_iterator;

  ConstDeclTable()                              = default;
  ConstDeclTable(const ConstDeclTable& rhs)     = delete;
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

  auto registerBound(TypeId type) -> ConstId;
  auto registerInput(std::string name, TypeId type) -> ConstId;
  auto registerLocal(std::string name, TypeId type) -> ConstId;

private:
  std::vector<ConstDecl> m_consts;
  std::unordered_map<std::string, ConstId> m_lookup;

  auto registerConst(ConstKind kind, std::string name, TypeId type) -> ConstId;

  [[nodiscard]] auto getConstIds(ConstKind beginKind, ConstKind endKind) const
      -> std::vector<ConstId>;
};

} // namespace prog::sym
