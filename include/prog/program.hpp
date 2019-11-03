#pragma once
#include "prog/sym/action_decl_table.hpp"
#include "prog/sym/exec_def.hpp"
#include "prog/sym/func_decl_table.hpp"
#include "prog/sym/func_def_table.hpp"
#include "prog/sym/type_decl_table.hpp"

namespace prog {

class Program final {
public:
  Program()                       = default;
  Program(const Program& rhs)     = delete;
  Program(Program&& rhs) noexcept = default;
  ~Program()                      = default;

  auto operator=(const Program& rhs) -> Program& = delete;
  auto operator=(Program&& rhs) noexcept -> Program& = delete;

  [[nodiscard]] auto lookupType(const std::string& name) const -> std::optional<sym::TypeId>;
  [[nodiscard]] auto lookupFunc(const std::string& name, const sym::Input& input) const
      -> std::optional<sym::FuncId>;
  [[nodiscard]] auto lookupFuncs(const std::string& name) const -> std::vector<sym::FuncId>;

  [[nodiscard]] auto lookupAction(const std::string& name, const sym::Input& input) const
      -> std::optional<sym::ActionId>;
  [[nodiscard]] auto lookupActions(const std::string& name) const -> std::vector<sym::ActionId>;

  auto declareUserFunc(std::string name, sym::FuncSig sig) -> void;
  auto defineUserFunc(sym::FuncId id, sym::ConstDeclTable consts, expr::NodePtr expr) -> void;
  auto addExec(sym::ActionId action, sym::ConstDeclTable consts, std::vector<expr::NodePtr> args)
      -> void;

private:
  sym::TypeDeclTable m_typeDecls;
  sym::FuncDeclTable m_funcDecls;
  sym::ActionDeclTable m_actionDecls;
  sym::FuncDefTable m_funcDefs;
  std::vector<sym::ExecDef> m_execs;
};

} // namespace prog
