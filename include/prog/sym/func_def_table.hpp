#pragma once
#include "prog/sym/const_decl_table.hpp"
#include "prog/sym/func_def.hpp"
#include "prog/sym/func_id.hpp"
#include "prog/sym/func_id_hasher.hpp"
#include <set>
#include <unordered_map>
#include <vector>

namespace prog::sym {

// Function definition table. Contains a defintiion for all user-functions.
class FuncDefTable final {
public:
  using Iterator = std::set<FuncId>::const_iterator;

  FuncDefTable()                        = default;
  FuncDefTable(const FuncDefTable& rhs) = delete;
  FuncDefTable(FuncDefTable&& rhs)      = default;
  ~FuncDefTable()                       = default;

  auto operator=(const FuncDefTable& rhs) -> FuncDefTable& = delete;
  auto operator=(FuncDefTable&& rhs) noexcept -> FuncDefTable& = default;

  [[nodiscard]] auto operator[](sym::FuncId id) const -> const FuncDef&;

  [[nodiscard]] auto begin() const -> Iterator;
  [[nodiscard]] auto end() const -> Iterator;

  auto registerFunc(
      const sym::FuncDeclTable& funcTable,
      sym::FuncId id,
      sym::ConstDeclTable consts,
      expr::NodePtr body,
      std::vector<expr::NodePtr> optArgInitializers) -> void;

private:
  std::unordered_map<FuncId, FuncDef, FuncIdHasher> m_funcDefs;
  std::set<FuncId> m_funcs;
};

} // namespace prog::sym
