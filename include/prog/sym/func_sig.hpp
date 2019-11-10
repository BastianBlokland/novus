#pragma once
#include "prog/sym/input.hpp"
#include "prog/sym/type_id.hpp"
#include <iostream>

namespace prog::sym {

class FuncSig final {
  friend auto operator<<(std::ostream& out, const FuncSig& rhs) -> std::ostream&;

public:
  FuncSig() = delete;
  FuncSig(Input input, TypeId output);

  auto operator==(const FuncSig& rhs) const noexcept -> bool;
  auto operator!=(const FuncSig& rhs) const noexcept -> bool;

  [[nodiscard]] auto getInput() const -> const Input&;
  [[nodiscard]] auto getOutput() const -> const TypeId&;

private:
  Input m_input;
  TypeId m_output;
};

auto operator<<(std::ostream& out, const FuncSig& rhs) -> std::ostream&;

} // namespace prog::sym
