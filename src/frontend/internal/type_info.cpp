#include "internal/type_info.hpp"

namespace frontend::internal {

TypeInfo::TypeInfo(std::string name, input::Span sourceSpan) :
    m_name{std::move(name)}, m_span{sourceSpan} {}

TypeInfo::TypeInfo(std::string name, input::Span sourceSpan, prog::sym::TypeSet params) :
    m_name{std::move(name)}, m_span{sourceSpan}, m_params{std::move(params)} {}

auto TypeInfo::getName() const noexcept -> const std::string& { return m_name; }

auto TypeInfo::getSourceSpan() const noexcept -> input::Span { return m_span; }

auto TypeInfo::hasParams() const noexcept -> bool { return m_params.has_value(); }

auto TypeInfo::getParams() const noexcept -> const std::optional<prog::sym::TypeSet>& {
  return m_params;
}

} // namespace frontend::internal
