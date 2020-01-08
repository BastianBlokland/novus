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

  auto visit(const prog::expr::CallDynExprNode & /*unused*/) -> void override {
    m_fg = rang::fg::yellow;
  }

  auto visit(const prog::expr::ClosureNode & /*unused*/) -> void override { m_fg = rang::fg::cyan; }

  auto visit(const prog::expr::ConstExprNode & /*unused*/) -> void override {
    m_fg = rang::fg::green;
  }

  auto visit(const prog::expr::FieldExprNode & /*unused*/) -> void override {
    m_fg = rang::fg::green;
  }

  auto visit(const prog::expr::GroupExprNode & /*unused*/) -> void override {
    m_fg = rang::fg::yellow;
  }

  auto visit(const prog::expr::UnionCheckExprNode & /*unused*/) -> void override {
    m_fg = rang::fg::green;
  }

  auto visit(const prog::expr::UnionGetExprNode & /*unused*/) -> void override {
    m_fg = rang::fg::green;
  }

  auto visit(const prog::expr::FailNode & /*unused*/) -> void override { m_fg = rang::fg::red; }

  auto visit(const prog::expr::LitBoolNode & /*unused*/) -> void override { m_fg = rang::fg::cyan; }

  auto visit(const prog::expr::LitFloatNode & /*unused*/) -> void override {
    m_fg = rang::fg::cyan;
  }

  auto visit(const prog::expr::LitFuncNode & /*unused*/) -> void override { m_fg = rang::fg::cyan; }

  auto visit(const prog::expr::LitIntNode & /*unused*/) -> void override { m_fg = rang::fg::cyan; }

  auto visit(const prog::expr::LitStringNode & /*unused*/) -> void override {
    m_fg = rang::fg::cyan;
  }

  auto visit(const prog::expr::LitCharNode & /*unused*/) -> void override { m_fg = rang::fg::cyan; }

private:
  rang::fg m_fg{};
};

} // namespace progdiag
