#pragma once
#include "prog/expr/node.hpp"
#include "prog/sym/const_decl_table.hpp"
#include "prog/sym/func_decl_table.hpp"

namespace prog::sym {

class FuncDef final {
  friend class FuncDefTable;

public:
  FuncDef()                       = delete;
  FuncDef(const FuncDef& rhs)     = delete;
  FuncDef(FuncDef&& rhs) noexcept = default;
  ~FuncDef()                      = default;

  auto operator=(const FuncDef& rhs) -> FuncDef& = delete;
  auto operator=(FuncDef&& rhs) noexcept -> FuncDef& = delete;

private:
  sym::FuncId m_id;
  sym::ConstDeclTable m_consts;
  expr::NodePtr m_expr;

  FuncDef(sym::FuncId id, sym::ConstDeclTable consts, expr::NodePtr expr);
};

} // namespace prog::sym
