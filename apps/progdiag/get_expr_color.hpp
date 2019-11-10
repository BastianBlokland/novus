#pragma once
#include "prog/expr/node_visitor.hpp"
#include "rang.hpp"

namespace progdiag {

class GetExprColor final : public prog::expr::NodeVisitor {
public:
  [[nodiscard]] auto getFgColor() const noexcept -> rang::fg { return m_fg; }

  auto visit(const prog::expr::AssignExprNode & /*unused*/) -> void override {
    m_fg = rang::fg::green;
  }

  auto visit(const prog::expr::SwitchExprNode & /*unused*/) -> void override {
    m_fg = rang::fg::magenta;
  }

  auto visit(const prog::expr::CallExprNode & /*unused*/) -> void override {
    m_fg = rang::fg::yellow;
  }

  auto visit(const prog::expr::ConstExprNode & /*unused*/) -> void override {
    m_fg = rang::fg::green;
  }

  auto visit(const prog::expr::GroupExprNode & /*unused*/) -> void override {
    m_fg = rang::fg::blue;
  }

  auto visit(const prog::expr::LitBoolNode & /*unused*/) -> void override { m_fg = rang::fg::cyan; }

  auto visit(const prog::expr::LitIntNode & /*unused*/) -> void override { m_fg = rang::fg::cyan; }

private:
  rang::fg m_fg{};
};

} // namespace progdiag
