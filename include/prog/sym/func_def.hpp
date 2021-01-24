#pragma once
#include "prog/expr/node.hpp"
#include "prog/sym/const_decl_table.hpp"
#include "prog/sym/func_decl_table.hpp"
#include <vector>

namespace prog::sym {

// Function definition. Contains the body of a user-function.
class FuncDef final {
  friend class FuncDefTable;

public:
  FuncDef()                   = delete;
  FuncDef(const FuncDef& rhs) = delete;
  FuncDef(FuncDef&& rhs)      = default;
  ~FuncDef()                  = default;

  auto operator=(const FuncDef& rhs) -> FuncDef& = delete;
  auto operator=(FuncDef&& rhs) noexcept -> FuncDef& = delete;

  [[nodiscard]] auto getId() const noexcept -> const FuncId&;
  [[nodiscard]] auto getConsts() const noexcept -> const sym::ConstDeclTable&;
  [[nodiscard]] auto getBody() const noexcept -> const expr::Node&;

private:
  sym::FuncId m_id;
  sym::ConstDeclTable m_consts;
  expr::NodePtr m_body;
  std::vector<expr::NodePtr> m_optArgInitializers;

  FuncDef(
      sym::FuncId id,
      sym::ConstDeclTable consts,
      expr::NodePtr body,
      std::vector<expr::NodePtr> optArgInitializers);
};

} // namespace prog::sym
