#pragma once
#include "prog/sym/action_decl_table.hpp"
#include "prog/sym/exec_stmt.hpp"
#include "prog/sym/func_decl_table.hpp"
#include "prog/sym/func_def_table.hpp"
#include "prog/sym/type_decl_table.hpp"
#include "prog/sym/type_def_table.hpp"

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
  using typeDefIterator    = typename sym::TypeDefTable::iterator;
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

  [[nodiscard]] auto beginTypeDefs() const -> typeDefIterator;
  [[nodiscard]] auto endTypeDefs() const -> typeDefIterator;

  [[nodiscard]] auto beginFuncDefs() const -> funcDefIterator;
  [[nodiscard]] auto endFuncDefs() const -> funcDefIterator;

  [[nodiscard]] auto beginExecStmts() const -> execStmtIterator;
  [[nodiscard]] auto endExecStmts() const -> execStmtIterator;

  [[nodiscard]] auto getInt() const noexcept -> sym::TypeId;
  [[nodiscard]] auto getFloat() const noexcept -> sym::TypeId;
  [[nodiscard]] auto getBool() const noexcept -> sym::TypeId;
  [[nodiscard]] auto getString() const noexcept -> sym::TypeId;
  [[nodiscard]] auto getChar() const noexcept -> sym::TypeId;

  [[nodiscard]] auto hasType(const std::string& name) const -> bool;
  [[nodiscard]] auto lookupType(const std::string& name) const -> std::optional<sym::TypeId>;

  [[nodiscard]] auto hasFunc(const std::string& name) const -> bool;
  [[nodiscard]] auto
  lookupFunc(const std::string& name, const sym::TypeSet& input, int maxConversions) const
      -> std::optional<sym::FuncId>;
  [[nodiscard]] auto lookupFunc(
      const std::vector<sym::FuncId>& funcs, const sym::TypeSet& input, int maxConversions) const
      -> std::optional<sym::FuncId>;
  [[nodiscard]] auto lookupFuncs(const std::string& name) const -> std::vector<sym::FuncId>;

  [[nodiscard]] auto
  lookupAction(const std::string& name, const sym::TypeSet& input, int maxConversions) const
      -> std::optional<sym::ActionId>;
  [[nodiscard]] auto lookupActions(const std::string& name) const -> std::vector<sym::ActionId>;

  [[nodiscard]] auto lookupConversion(sym::TypeId from, sym::TypeId to) const
      -> std::optional<sym::FuncId>;

  [[nodiscard]] auto findCommonType(const std::vector<sym::TypeId>& types)
      -> std::optional<sym::TypeId>;

  [[nodiscard]] auto isDelegate(sym::TypeId id) const -> bool;

  [[nodiscard]] auto isCallable(sym::FuncId func, const std::vector<expr::NodePtr>& args) const
      -> bool;
  [[nodiscard]] auto isCallable(sym::TypeId delegate, const std::vector<expr::NodePtr>& args) const
      -> bool;

  [[nodiscard]] auto getTypeDecl(sym::TypeId id) const -> const sym::TypeDecl&;
  [[nodiscard]] auto getFuncDecl(sym::FuncId id) const -> const sym::FuncDecl&;
  [[nodiscard]] auto getActionDecl(sym::ActionId id) const -> const sym::ActionDecl&;

  [[nodiscard]] auto hasTypeDef(sym::TypeId id) const -> bool;
  [[nodiscard]] auto getTypeDef(sym::TypeId id) const -> const sym::TypeDefTable::typeDef&;
  [[nodiscard]] auto getFuncDef(sym::FuncId id) const -> const sym::FuncDef&;

  auto declareUserStruct(std::string name) -> sym::TypeId;
  auto declareUserUnion(std::string name) -> sym::TypeId;
  auto declareUserDelegate(std::string name) -> sym::TypeId;
  auto declareUserFunc(std::string name, sym::TypeSet input, sym::TypeId output) -> sym::FuncId;

  auto defineUserStruct(sym::TypeId id, sym::FieldDeclTable fields) -> void;
  auto defineUserUnion(sym::TypeId id, std::vector<sym::TypeId> types) -> void;
  auto defineUserDelegate(sym::TypeId id, sym::TypeSet input, sym::TypeId output) -> void;
  auto defineUserFunc(sym::FuncId id, sym::ConstDeclTable consts, expr::NodePtr expr) -> void;

  auto
  addExecStmt(sym::ActionId action, sym::ConstDeclTable consts, std::vector<expr::NodePtr> args)
      -> void;

  auto updateFuncOutput(sym::FuncId funcId, sym::TypeId newOutput) -> void;

private:
  sym::TypeDeclTable m_typeDecls;
  sym::FuncDeclTable m_funcDecls;
  sym::ActionDeclTable m_actionDecls;
  sym::TypeDefTable m_typeDefs;
  sym::FuncDefTable m_funcDefs;
  std::vector<sym::ExecStmt> m_execStmts;

  sym::TypeId m_int;
  sym::TypeId m_float;
  sym::TypeId m_bool;
  sym::TypeId m_string;
  sym::TypeId m_char;
};

} // namespace prog
