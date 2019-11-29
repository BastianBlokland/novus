#pragma once
#include "prog/sym/func_id.hpp"
#include "prog/sym/func_kind.hpp"
#include "prog/sym/input.hpp"
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
  [[nodiscard]] auto getName() const -> const std::string&;
  [[nodiscard]] auto getInput() const -> const Input&;
  [[nodiscard]] auto getOutput() const -> TypeId;

  auto updateOutput(TypeId newOutput) -> void;

private:
  FuncId m_id;
  FuncKind m_kind;
  std::string m_name;
  Input m_input;
  TypeId m_output;

  FuncDecl(FuncId id, FuncKind kind, std::string name, Input input, TypeId output);
};

auto operator<<(std::ostream& out, const FuncDecl& rhs) -> std::ostream&;

} // namespace prog::sym
