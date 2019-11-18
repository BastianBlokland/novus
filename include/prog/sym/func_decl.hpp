#pragma once
#include "prog/sym/func_id.hpp"
#include "prog/sym/func_kind.hpp"
#include "prog/sym/func_sig.hpp"
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
  [[nodiscard]] auto getSig() const -> const FuncSig&;

  auto updateSig(FuncSig newSig) -> void;

private:
  FuncId m_id;
  FuncKind m_kind;
  std::string m_name;
  FuncSig m_sig;

  FuncDecl(FuncId id, FuncKind kind, std::string name, FuncSig sig);
};

auto operator<<(std::ostream& out, const FuncDecl& rhs) -> std::ostream&;

} // namespace prog::sym
