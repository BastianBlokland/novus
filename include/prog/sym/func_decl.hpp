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

  auto operator==(const FuncDecl& rhs) const noexcept -> bool { return m_id == rhs.m_id; }
  auto operator!=(const FuncDecl& rhs) const noexcept -> bool { return !FuncDecl::operator==(rhs); }

  [[nodiscard]] auto getId() const noexcept -> const FuncId& { return m_id; }
  [[nodiscard]] auto getKind() const noexcept -> const FuncKind& { return m_kind; }
  [[nodiscard]] auto isAction() const noexcept -> bool { return m_isAction; }
  [[nodiscard]] auto isIntrinsic() const noexcept -> bool { return m_isIntrinsic; }
  [[nodiscard]] auto isImplicitConv() const noexcept -> bool { return m_isImplicitConv; }
  [[nodiscard]] auto getName() const noexcept -> const std::string& { return m_name; }
  [[nodiscard]] auto getInput() const noexcept -> const TypeSet& { return m_input; }
  [[nodiscard]] auto getOutput() const noexcept -> TypeId { return m_output; }
  [[nodiscard]] auto getMinInputCount() const noexcept -> unsigned int {
    return m_input.getCount() - m_numOptInputs;
  }
  [[nodiscard]] auto getNumOptInputs() const noexcept -> unsigned int { return m_numOptInputs; }

  auto updateOutput(TypeId newOutput) noexcept -> void { m_output = newOutput; }

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
      unsigned int numOptInputs) :
      m_id{id},
      m_kind{kind},
      m_isAction{isAction},
      m_isIntrinsic{isIntrinsic},
      m_isImplicitConv{isImplicitConv},
      m_name{std::move(name)},
      m_input{std::move(input)},
      m_output{output},
      m_numOptInputs{numOptInputs} {}
};

inline auto operator<<(std::ostream& out, const FuncDecl& rhs) -> std::ostream& {
  return out << '[' << rhs.m_id << "]-'" << rhs.m_name << "'-" << '(' << rhs.m_input << ")->"
             << rhs.m_output;
}

} // namespace prog::sym
