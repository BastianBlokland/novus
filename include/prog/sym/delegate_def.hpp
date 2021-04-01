#pragma once
#include "prog/sym/type_id.hpp"
#include "prog/sym/type_set.hpp"

namespace prog::sym {

// Definition for a delegate type. Contains the signature of the functions it represents.
// Note: 'isAction' indicates if this delegate is pure or not.
class DelegateDef final {
  friend class TypeDefTable;

public:
  DelegateDef()                           = delete;
  DelegateDef(const DelegateDef& rhs)     = default;
  DelegateDef(DelegateDef&& rhs) noexcept = default;
  ~DelegateDef()                          = default;

  auto operator=(const DelegateDef& rhs) -> DelegateDef& = delete;
  auto operator=(DelegateDef&& rhs) noexcept -> DelegateDef& = delete;

  [[nodiscard]] auto getId() const noexcept -> const TypeId& { return m_id; }
  [[nodiscard]] auto isAction() const noexcept -> bool { return m_isAction; }
  [[nodiscard]] auto getInput() const -> const TypeSet& { return m_input; }
  [[nodiscard]] auto getOutput() const -> TypeId { return m_output; }

private:
  sym::TypeId m_id;
  bool m_isAction;
  TypeSet m_input;
  TypeId m_output;

  DelegateDef(sym::TypeId id, bool isAction, TypeSet input, TypeId output) :
      m_id{id}, m_isAction{isAction}, m_input{std::move(input)}, m_output{output} {}
};

} // namespace prog::sym
