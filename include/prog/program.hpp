#pragma once
#include "prog/sym/action_decl_table.hpp"
#include "prog/sym/exec_stmt.hpp"
#include "prog/sym/func_decl_table.hpp"
#include "prog/sym/func_def_table.hpp"
#include "prog/sym/type_decl_table.hpp"

namespace prog {

class Program;

namespace internal {

[[nodiscard]] auto getTypeDeclTable(const Program& prog) -> const sym::TypeDeclTable&;
[[nodiscard]] auto getFuncDeclTable(const Program& prog) -> const sym::FuncDeclTable&;
[[nodiscard]] auto getActionDeclTable(const Program& prog) -> const sym::ActionDeclTable&;

} // namespace internal

class Program final {
  friend auto internal::getTypeDeclTable(const Program& prog) -> const sym::TypeDeclTable&;
  friend auto internal::getFuncDeclTable(const Program& prog) -> const sym::FuncDeclTable&;
  friend auto internal::getActionDeclTable(const Program& prog) -> const sym::ActionDeclTable&;

public:
  using typeDeclIterator   = typename sym::TypeDeclTable::iterator;
  using funcDeclIterator   = typename sym::FuncDeclTable::iterator;
  using actionDeclIterator = typename sym::ActionDeclTable::iterator;
  using funcDefIterator    = typename sym::FuncDefTable::iterator;
  using execStmtIterator   = typename std::vector<sym::ExecStmt>::const_iterator;

  Program();
  Program(const Program& rhs)     = delete;
  Program(Program&& rhs) noexcept = default;
  ~Program()                      = default;

  auto operator=(const Program& rhs) -> Program& = delete;
  auto operator=(Program&& rhs) noexcept -> Program& = delete;

  [[nodiscard]] auto beginTypeDecls() const -> typeDeclIterator;
  [[nodiscard]] auto endTypeDecls() const -> typeDeclIterator;

  [[nodiscard]] auto beginFuncDecls() const -> funcDeclIterator;
  [[nodiscard]] auto endFuncDecls() const -> funcDeclIterator;

  [[nodiscard]] auto beginActionDecls() const -> actionDeclIterator;
  [[nodiscard]] auto endActionDecls() const -> actionDeclIterator;

  [[nodiscard]] auto beginFuncDefs() const -> funcDefIterator;
  [[nodiscard]] auto endFuncDefs() const -> funcDefIterator;

  [[nodiscard]] auto beginExecStmts() const -> execStmtIterator;
  [[nodiscard]] auto endExecStmts() const -> execStmtIterator;

  [[nodiscard]] auto lookupType(const std::string& name) const -> std::optional<sym::TypeId>;
  [[nodiscard]] auto
  lookupFunc(const std::string& name, const sym::Input& input, int maxConversions) const
      -> std::optional<sym::FuncId>;
  [[nodiscard]] auto lookupFuncs(const std::string& name) const -> std::vector<sym::FuncId>;

  [[nodiscard]] auto
  lookupAction(const std::string& name, const sym::Input& input, int maxConversions) const
      -> std::optional<sym::ActionId>;
  [[nodiscard]] auto lookupActions(const std::string& name) const -> std::vector<sym::ActionId>;

  [[nodiscard]] auto lookupConversion(sym::TypeId from, sym::TypeId to) const
      -> std::optional<sym::FuncId>;

  [[nodiscard]] auto getTypeDecl(sym::TypeId id) const -> const sym::TypeDecl&;
  [[nodiscard]] auto getFuncDecl(sym::FuncId id) const -> const sym::FuncDecl&;
  [[nodiscard]] auto getActionDecl(sym::ActionId id) const -> const sym::ActionDecl&;
  [[nodiscard]] auto getFuncDef(sym::FuncId id) const -> const sym::FuncDef&;

  auto declareUserStruct(std::string name) -> sym::TypeId;
  auto declareUserFunc(std::string name, sym::Input input, sym::TypeId output) -> sym::FuncId;
  auto defineUserFunc(sym::FuncId id, sym::ConstDeclTable consts, expr::NodePtr expr) -> void;
  auto
  addExecStmt(sym::ActionId action, sym::ConstDeclTable consts, std::vector<expr::NodePtr> args)
      -> void;

  auto updateFuncOutput(sym::FuncId funcId, sym::TypeId newOutput) -> void;

private:
  sym::TypeDeclTable m_typeDecls;
  sym::FuncDeclTable m_funcDecls;
  sym::ActionDeclTable m_actionDecls;
  sym::FuncDefTable m_funcDefs;
  std::vector<sym::ExecStmt> m_execStmts;
};

} // namespace prog
