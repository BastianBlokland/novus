#pragma once
#include "prog/sym/exec_stmt.hpp"
#include "prog/sym/func_decl_table.hpp"
#include "prog/sym/func_def_table.hpp"
#include "prog/sym/overload_options.hpp"
#include "prog/sym/type_decl_table.hpp"
#include "prog/sym/type_def_table.hpp"

namespace prog {

using OvFlags   = typename sym::OverloadFlags;
using OvOptions = typename sym::OverloadOptions;

class Program;

namespace internal {

[[nodiscard]] auto getTypeDeclTable(const Program& prog) -> const sym::TypeDeclTable&;
[[nodiscard]] auto getFuncDeclTable(const Program& prog) -> const sym::FuncDeclTable&;

} // namespace internal

class Program final {
  friend auto internal::getTypeDeclTable(const Program& prog) -> const sym::TypeDeclTable&;
  friend auto internal::getFuncDeclTable(const Program& prog) -> const sym::FuncDeclTable&;

public:
  using typeDeclIterator = typename sym::TypeDeclTable::iterator;
  using funcDeclIterator = typename sym::FuncDeclTable::iterator;
  using typeDefIterator  = typename sym::TypeDefTable::iterator;
  using funcDefIterator  = typename sym::FuncDefTable::iterator;
  using execStmtIterator = typename std::vector<sym::ExecStmt>::const_iterator;

  Program();
  Program(const Program& rhs)     = delete;
  Program(Program&& rhs) noexcept = default;
  ~Program()                      = default;

  auto operator=(const Program& rhs) -> Program& = delete;
  auto operator=(Program&& rhs) noexcept -> Program& = delete;

  [[nodiscard]] auto beginTypeDecls() const -> typeDeclIterator;
  [[nodiscard]] auto endTypeDecls() const -> typeDeclIterator;

  [[nodiscard]] auto getFuncCount() const -> unsigned int;
  [[nodiscard]] auto beginFuncDecls() const -> funcDeclIterator;
  [[nodiscard]] auto endFuncDecls() const -> funcDeclIterator;

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
  lookupFunc(const std::string& name, const sym::TypeSet& input, OvOptions options) const
      -> std::optional<sym::FuncId>;

  [[nodiscard]] auto lookupFunc(
      const std::vector<sym::FuncId>& funcs, const sym::TypeSet& input, OvOptions options) const
      -> std::optional<sym::FuncId>;

  [[nodiscard]] auto lookupFuncs(const std::string& name, OvOptions options) const
      -> std::vector<sym::FuncId>;

  [[nodiscard]] auto lookupImplicitConv(sym::TypeId from, sym::TypeId to) const
      -> std::optional<sym::FuncId>;

  [[nodiscard]] auto
  isImplicitConvertible(const sym::TypeSet& toTypes, const sym::TypeSet& fromTypes) const -> bool;

  [[nodiscard]] auto findCommonType(const std::vector<sym::TypeId>& types)
      -> std::optional<sym::TypeId>;

  [[nodiscard]] auto isDelegate(sym::TypeId id) const -> bool;

  [[nodiscard]] auto isCallable(sym::FuncId func, const std::vector<expr::NodePtr>& args) const
      -> bool;
  [[nodiscard]] auto isCallable(sym::TypeId delegate, const std::vector<expr::NodePtr>& args) const
      -> bool;

  [[nodiscard]] auto getTypeDecl(sym::TypeId id) const -> const sym::TypeDecl&;
  [[nodiscard]] auto getFuncDecl(sym::FuncId id) const -> const sym::FuncDecl&;

  [[nodiscard]] auto hasTypeDef(sym::TypeId id) const -> bool;
  [[nodiscard]] auto getTypeDef(sym::TypeId id) const -> const sym::TypeDefTable::typeDef&;
  [[nodiscard]] auto getFuncDef(sym::FuncId id) const -> const sym::FuncDef&;

  auto declareStruct(std::string name) -> sym::TypeId;
  auto declareUnion(std::string name) -> sym::TypeId;
  auto declareEnum(std::string name) -> sym::TypeId;
  auto declareDelegate(std::string name) -> sym::TypeId;
  auto declarePureFunc(std::string name, sym::TypeSet input, sym::TypeId output) -> sym::FuncId;
  auto declareAction(std::string name, sym::TypeSet input, sym::TypeId output) -> sym::FuncId;

  auto defineStruct(sym::TypeId id, sym::FieldDeclTable fields) -> void;
  auto defineUnion(sym::TypeId id, std::vector<sym::TypeId> types) -> void;
  auto defineEnum(sym::TypeId id, std::unordered_map<std::string, int32_t> entries) -> void;
  auto defineDelegate(sym::TypeId id, sym::TypeSet input, sym::TypeId output) -> void;
  auto defineFunc(sym::FuncId id, sym::ConstDeclTable consts, expr::NodePtr expr) -> void;

  auto addExecStmt(sym::ConstDeclTable consts, expr::NodePtr expr) -> void;

  auto updateFuncOutput(sym::FuncId funcId, sym::TypeId newOutput) -> void;

private:
  sym::TypeDeclTable m_typeDecls;
  sym::FuncDeclTable m_funcDecls;
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
