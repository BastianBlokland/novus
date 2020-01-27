#pragma once
#include "prog/sym/func_id.hpp"
#include "prog/sym/func_kind.hpp"
#include "prog/sym/type_set.hpp"
#include <string>
#include <vector>

namespace prog::sym {

class FuncDecl final {
  friend class FuncDeclTable;
  friend auto operator<<(std::ostream& out, const FuncDecl& rhs) -> std::ostream&;

public:
  FuncDecl() = delete;

  auto operator==(const FuncDecl& rhs) const noexcept -> bool;
  auto operator!=(const FuncDecl& rhs) const noexcept -> bool;

  [[nodiscard]] auto getId() const -> const FuncId&;
  [[nodiscard]] auto getKind() const -> const FuncKind&;
  [[nodiscard]] auto isAction() const -> bool;
  [[nodiscard]] auto isImplicitConv() const -> bool;
  [[nodiscard]] auto getName() const -> const std::string&;
  [[nodiscard]] auto getInput() const -> const TypeSet&;
  [[nodiscard]] auto getOutput() const -> TypeId;

  auto updateOutput(TypeId newOutput) -> void;

private:
  FuncId m_id;
  FuncKind m_kind;
  bool m_isAction;
  bool m_isImplicitConv;
  std::string m_name;
  TypeSet m_input;
  TypeId m_output;

  FuncDecl(
      FuncId id,
      FuncKind kind,
      bool isAction,
      bool isImplicitConv,
      std::string name,
      TypeSet input,
      TypeId output);
};

auto operator<<(std::ostream& out, const FuncDecl& rhs) -> std::ostream&;

} // namespace prog::sym
