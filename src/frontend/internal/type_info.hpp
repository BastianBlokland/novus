#pragma once
#include "input/span.hpp"
#include "prog/sym/type_set.hpp"
#include <optional>

namespace frontend::internal {

class TypeInfo final {
public:
  TypeInfo() = delete;
  explicit TypeInfo(std::string name, input::Span sourceSpan);
  explicit TypeInfo(std::string name, input::Span sourceSpan, prog::sym::TypeSet params);

  [[nodiscard]] auto getName() const noexcept -> const std::string&;
  [[nodiscard]] auto getSourceSpan() const noexcept -> input::Span;
  [[nodiscard]] auto hasParams() const noexcept -> bool;
  [[nodiscard]] auto getParams() const noexcept -> const std::optional<prog::sym::TypeSet>&;

private:
  std::string m_name;
  input::Span m_span;
  std::optional<prog::sym::TypeSet> m_params;
};

} // namespace frontend::internal
