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

[[nodiscard]] auto getTypeDeclTable(Program* prog) -> sym::TypeDeclTable&;
[[nodiscard]] auto getFuncDeclTable(Program* prog) -> sym::FuncDeclTable&;

[[nodiscard]] auto getTypeDefTable(Program* prog) -> sym::TypeDefTable&;
[[nodiscard]] auto getFuncDefTable(Program* prog) -> sym::FuncDefTable&;

} // namespace internal

class Program final {
  friend auto internal::getTypeDeclTable(const Program& prog) -> const sym::TypeDeclTable&;
  friend auto internal::getFuncDeclTable(const Program& prog) -> const sym::FuncDeclTable&;
  friend auto internal::getTypeDeclTable(Program* prog) -> sym::TypeDeclTable&;
  friend auto internal::getFuncDeclTable(Program* prog) -> sym::FuncDeclTable&;
  friend auto internal::getTypeDefTable(Program* prog) -> sym::TypeDefTable&;
  friend auto internal::getFuncDefTable(Program* prog) -> sym::FuncDefTable&;

public:
  using TypeDeclIterator = typename sym::TypeDeclTable::Iterator;
  using FuncDeclIterator = typename sym::FuncDeclTable::Iterator;
  using TypeDefIterator  = typename sym::TypeDefTable::Iterator;
  using FuncDefIterator  = typename sym::FuncDefTable::Iterator;
  using ExecStmtIterator = typename std::vector<sym::ExecStmt>::const_iterator;

  Program();
  Program(const Program& rhs)     = delete;
  Program(Program&& rhs) noexcept = default;
  ~Program()                      = default;

  auto operator=(const Program& rhs) -> Program& = delete;
  auto operator=(Program&& rhs) noexcept -> Program& = delete;

  [[nodiscard]] auto beginTypeDecls() const -> TypeDeclIterator { return m_typeDecls.begin(); }
  [[nodiscard]] auto endTypeDecls() const -> TypeDeclIterator { return m_typeDecls.end(); }

  [[nodiscard]] auto getFuncCount() const -> unsigned int { return m_funcDecls.getFuncCount(); }
  [[nodiscard]] auto beginFuncDecls() const -> FuncDeclIterator { return m_funcDecls.begin(); }
  [[nodiscard]] auto endFuncDecls() const -> FuncDeclIterator { return m_funcDecls.end(); }

  [[nodiscard]] auto beginTypeDefs() const -> TypeDefIterator { return m_typeDefs.begin(); }
  [[nodiscard]] auto endTypeDefs() const -> TypeDefIterator { return m_typeDefs.end(); }

  [[nodiscard]] auto beginFuncDefs() const -> FuncDefIterator { return m_funcDefs.begin(); }
  [[nodiscard]] auto endFuncDefs() const -> FuncDefIterator { return m_funcDefs.end(); }

  [[nodiscard]] auto beginExecStmts() const -> ExecStmtIterator { return m_execStmts.begin(); }
  [[nodiscard]] auto endExecStmts() const -> ExecStmtIterator { return m_execStmts.end(); }

  [[nodiscard]] auto getInt() const noexcept -> sym::TypeId { return m_int; }
  [[nodiscard]] auto getLong() const noexcept -> sym::TypeId { return m_long; }
  [[nodiscard]] auto getFloat() const noexcept -> sym::TypeId { return m_float; }
  [[nodiscard]] auto getBool() const noexcept -> sym::TypeId { return m_bool; }
  [[nodiscard]] auto getString() const noexcept -> sym::TypeId { return m_string; }
  [[nodiscard]] auto getChar() const noexcept -> sym::TypeId { return m_char; }
  [[nodiscard]] auto getStream() const noexcept -> sym::TypeId { return m_stream; }

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

  [[nodiscard]] auto isFuture(sym::TypeId id) const -> bool;

  [[nodiscard]] auto satisfiesOptions(sym::TypeId delegate, OvOptions options) const -> bool;

  [[nodiscard]] auto
  isCallable(sym::TypeId delegate, const sym::TypeSet& input, OvOptions options) const -> bool;

  [[nodiscard]] auto getDelegateRetType(sym::TypeId id) const -> std::optional<sym::TypeId>;

  [[nodiscard]] auto getTypeDecl(sym::TypeId id) const -> const sym::TypeDecl&;
  [[nodiscard]] auto getFuncDecl(sym::FuncId id) const -> const sym::FuncDecl&;

  [[nodiscard]] auto hasTypeDef(sym::TypeId id) const -> bool;
  [[nodiscard]] auto getTypeDef(sym::TypeId id) const -> const sym::TypeDefTable::TypeDef&;
  [[nodiscard]] auto getFuncDef(sym::FuncId id) const -> const sym::FuncDef&;

  auto declareStruct(std::string name) -> sym::TypeId;
  auto declareUnion(std::string name) -> sym::TypeId;
  auto declareEnum(std::string name) -> sym::TypeId;
  auto declareDelegate(std::string name) -> sym::TypeId;
  auto declareFuture(std::string name) -> sym::TypeId;
  auto declarePureFunc(std::string name, sym::TypeSet input, sym::TypeId output) -> sym::FuncId;
  auto declareAction(std::string name, sym::TypeSet input, sym::TypeId output) -> sym::FuncId;

  auto defineStruct(sym::TypeId id, sym::FieldDeclTable fields) -> void;
  auto defineUnion(sym::TypeId id, std::vector<sym::TypeId> types) -> void;
  auto defineEnum(sym::TypeId id, std::unordered_map<std::string, int32_t> entries) -> void;
  auto defineDelegate(
      sym::TypeId id,
      bool isAction,
      sym::TypeSet input,
      sym::TypeId output,
      const std::vector<sym::TypeId>& aliases) -> void;
  auto defineFuture(sym::TypeId id, sym::TypeId result) -> void;
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
  sym::TypeId m_long;
  sym::TypeId m_float;
  sym::TypeId m_bool;
  sym::TypeId m_string;
  sym::TypeId m_char;
  sym::TypeId m_stream;
};

} // namespace prog
