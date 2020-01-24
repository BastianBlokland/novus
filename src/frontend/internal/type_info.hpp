#pragma once
#include "input/span.hpp"
#include "prog/sym/type_set.hpp"
#include <optional>

namespace frontend::internal {

class Context;

class TypeInfo final {
public:
  TypeInfo() = delete;
  TypeInfo(Context* ctx, std::string name, input::Span sourceSpan) :
      m_ctx{ctx}, m_name{std::move(name)}, m_span{sourceSpan} {}

  TypeInfo(Context* ctx, std::string name, input::Span sourceSpan, prog::sym::TypeSet params) :
      m_ctx{ctx}, m_name{std::move(name)}, m_span{sourceSpan}, m_params{std::move(params)} {}

  [[nodiscard]] auto getCtx() const noexcept -> Context* { return m_ctx; }
  [[nodiscard]] auto getName() const noexcept -> const std::string& { return m_name; };
  [[nodiscard]] auto getSourceSpan() const noexcept -> input::Span { return m_span; };
  [[nodiscard]] auto hasParams() const noexcept -> bool { return m_params.has_value(); };
  [[nodiscard]] auto getParams() const noexcept -> const std::optional<prog::sym::TypeSet>& {
    return m_params;
  }

private:
  Context* m_ctx;
  std::string m_name;
  input::Span m_span;
  std::optional<prog::sym::TypeSet> m_params;
};

} // namespace frontend::internal
