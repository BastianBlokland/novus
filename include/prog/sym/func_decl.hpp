#pragma once
#include "prog/sym/func_id.hpp"
#include "prog/sym/func_kind.hpp"
#include "prog/sym/type_set.hpp"
#include <string>
#include <vector>

namespace prog::sym {

// Function declaration. Combines the function-identifier, name and the function signature.
class FuncDecl final {
  friend class FuncDeclTable;
  friend auto operator<<(std::ostream& out, const FuncDecl& rhs) -> std::ostream&;

public:
  FuncDecl() = delete;

  auto operator==(const FuncDecl& rhs) const noexcept -> bool;
  auto operator!=(const FuncDecl& rhs) const noexcept -> bool;

  [[nodiscard]] auto getId() const noexcept -> const FuncId&;
  [[nodiscard]] auto getKind() const noexcept -> const FuncKind&;
  [[nodiscard]] auto isAction() const noexcept -> bool;
  [[nodiscard]] auto isIntrinsic() const noexcept -> bool;
  [[nodiscard]] auto isImplicitConv() const noexcept -> bool;
  [[nodiscard]] auto getName() const noexcept -> const std::string&;
  [[nodiscard]] auto getInput() const noexcept -> const TypeSet&;
  [[nodiscard]] auto getOutput() const noexcept -> TypeId;
  [[nodiscard]] auto getMinInputCount() const noexcept -> unsigned int;
  [[nodiscard]] auto getNumOptInputs() const noexcept -> unsigned int;

  auto updateOutput(TypeId newOutput) noexcept -> void;

private:
  FuncId m_id;
  FuncKind m_kind;
  bool m_isAction;
  bool m_isIntrinsic;
  bool m_isImplicitConv;
  std::string m_name;
  TypeSet m_input;
  TypeId m_output;
  unsigned int m_numOptInputs;

  FuncDecl(
      FuncId id,
      FuncKind kind,
      bool isAction,
      bool isIntrinsic,
      bool isImplicitConv,
      std::string name,
      TypeSet input,
      TypeId output,
      unsigned int m_numOptInputs);
};

auto operator<<(std::ostream& out, const FuncDecl& rhs) -> std::ostream&;

} // namespace prog::sym
