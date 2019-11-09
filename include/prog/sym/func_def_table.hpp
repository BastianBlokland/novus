#pragma once
#include "prog/sym/const_decl_table.hpp"
#include "prog/sym/func_def.hpp"
#include "prog/sym/func_id.hpp"
#include "prog/sym/func_id_hasher.hpp"
#include <unordered_map>
#include <vector>

namespace prog::sym {

class FuncDefTable final {
public:
  using iterator = typename std::unordered_map<FuncId, FuncDef>::const_iterator;

  FuncDefTable()                            = default;
  FuncDefTable(const FuncDefTable& rhs)     = delete;
  FuncDefTable(FuncDefTable&& rhs) noexcept = default;
  ~FuncDefTable()                           = default;

  auto operator=(const FuncDefTable& rhs) -> FuncDefTable& = delete;
  auto operator=(FuncDefTable&& rhs) noexcept -> FuncDefTable& = delete;

  [[nodiscard]] auto operator[](sym::FuncId id) const -> const FuncDef&;

  [[nodiscard]] auto begin() const -> iterator;
  [[nodiscard]] auto end() const -> iterator;

  auto registerFunc(
      const sym::FuncDeclTable& funcTable,
      sym::FuncId id,
      sym::ConstDeclTable consts,
      expr::NodePtr expr) -> void;

private:
  std::unordered_map<FuncId, FuncDef, FuncIdHasher> m_funcs;
};

} // namespace prog::sym
