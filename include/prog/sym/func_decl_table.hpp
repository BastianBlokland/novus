#pragma once
#include "prog/sym/func_decl.hpp"
#include "prog/sym/func_id.hpp"
#include "prog/sym/input.hpp"
#include "prog/sym/type_decl.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace prog::sym {

class FuncDeclTable final {
public:
  using iterator = typename std::vector<FuncDecl>::const_iterator;

  FuncDeclTable()                             = default;
  FuncDeclTable(const FuncDeclTable& rhs)     = delete;
  FuncDeclTable(FuncDeclTable&& rhs) noexcept = default;
  ~FuncDeclTable()                            = default;

  auto operator=(const FuncDeclTable& rhs) -> FuncDeclTable& = delete;
  auto operator=(FuncDeclTable&& rhs) noexcept -> FuncDeclTable& = delete;

  [[nodiscard]] auto operator[](FuncId id) const -> const FuncDecl&;

  [[nodiscard]] auto begin() const -> iterator;
  [[nodiscard]] auto end() const -> iterator;

  [[nodiscard]] auto lookup(const std::string& name) const -> std::vector<FuncId>;
  [[nodiscard]] auto lookup(const std::string& name, const Input& input) const
      -> std::optional<FuncId>;

  auto registerFunc(FuncKind kind, std::string name, FuncSig sig) -> FuncId;

private:
  std::vector<FuncDecl> m_funcs;
  std::unordered_map<std::string, std::vector<FuncId>> m_lookup;

  [[nodiscard]] auto findOverload(const std::vector<FuncId>& overloads, const Input& input) const
      -> std::optional<FuncId>;
};

} // namespace prog::sym
