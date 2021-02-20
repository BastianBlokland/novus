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
  enum class Flags : unsigned int {
    None     = 0U,
    NoInline = 1U << 0U, // Hint to the optmizer that it should not inline this function.
  };

  FuncDef()                   = delete;
  FuncDef(const FuncDef& rhs) = delete;
  FuncDef(FuncDef&& rhs)      = default;
  ~FuncDef()                  = default;

  auto operator=(const FuncDef& rhs) -> FuncDef& = delete;
  auto operator=(FuncDef&& rhs) noexcept -> FuncDef& = delete;

  [[nodiscard]] auto getId() const noexcept -> const FuncId&;
  [[nodiscard]] auto getFlags() const noexcept -> Flags;
  [[nodiscard]] auto hasFlags(Flags flags) const noexcept -> bool;
  [[nodiscard]] auto getConsts() const noexcept -> const sym::ConstDeclTable&;
  [[nodiscard]] auto getBody() const noexcept -> const expr::Node&;
  [[nodiscard]] auto getOptArgInitializer(unsigned int i) const -> expr::NodePtr;

private:
  sym::FuncId m_id;
  sym::ConstDeclTable m_consts;
  expr::NodePtr m_body;
  std::vector<expr::NodePtr> m_optArgInitializers;
  Flags m_flags;

  FuncDef(
      sym::FuncId id,
      sym::ConstDeclTable consts,
      expr::NodePtr body,
      std::vector<expr::NodePtr> optArgInitializers,
      Flags flags);
};

} // namespace prog::sym
