#pragma once
#include "parse/node_visitor.hpp"
#include "rang.hpp"

namespace parsediag {

class GetExprColor final : public parse::NodeVisitor {
public:
  [[nodiscard]] auto getFgColor() const noexcept -> rang::fg { return m_fg; }

  [[nodiscard]] auto getBgColor() const noexcept -> rang::bg { return m_bg; }

  auto visit(const parse::ErrorNode & /*unused*/) -> void override {
    m_fg = rang::fg::reset;
    m_bg = rang::bg::red;
  }

  auto visit(const parse::BinaryExprNode & /*unused*/) -> void override {
    m_fg = rang::fg::green;
    m_bg = rang::bg::reset;
  }

  auto visit(const parse::CallExprNode & /*unused*/) -> void override {
    m_fg = rang::fg::magenta;
    m_bg = rang::bg::reset;
  }

  auto visit(const parse::ConditionalExprNode & /*unused*/) -> void override {
    m_fg = rang::fg::green;
    m_bg = rang::bg::reset;
  }

  auto visit(const parse::ConstDeclExprNode & /*unused*/) -> void override {
    m_fg = rang::fg::magenta;
    m_bg = rang::bg::reset;
  }

  auto visit(const parse::ConstExprNode & /*unused*/) -> void override {
    m_fg = rang::fg::magenta;
    m_bg = rang::bg::reset;
  }

  auto visit(const parse::FieldExprNode & /*unused*/) -> void override {
    m_fg = rang::fg::magenta;
    m_bg = rang::bg::reset;
  }

  auto visit(const parse::GroupExprNode & /*unused*/) -> void override {
    m_fg = rang::fg::green;
    m_bg = rang::bg::reset;
  }

  auto visit(const parse::IndexExprNode & /*unused*/) -> void override {
    m_fg = rang::fg::magenta;
    m_bg = rang::bg::reset;
  }

  auto visit(const parse::IsExprNode & /*unused*/) -> void override {
    m_fg = rang::fg::yellow;
    m_bg = rang::bg::reset;
  }

  auto visit(const parse::LitExprNode & /*unused*/) -> void override {
    m_fg = rang::fg::cyan;
    m_bg = rang::bg::reset;
  }

  auto visit(const parse::ParenExprNode & /*unused*/) -> void override {
    m_fg = rang::fg::green;
    m_bg = rang::bg::reset;
  }

  auto visit(const parse::SwitchExprElseNode & /*unused*/) -> void override {
    m_fg = rang::fg::green;
    m_bg = rang::bg::reset;
  }

  auto visit(const parse::SwitchExprIfNode & /*unused*/) -> void override {
    m_fg = rang::fg::green;
    m_bg = rang::bg::reset;
  }

  auto visit(const parse::SwitchExprNode & /*unused*/) -> void override {
    m_fg = rang::fg::green;
    m_bg = rang::bg::reset;
  }

  auto visit(const parse::UnaryExprNode & /*unused*/) -> void override {
    m_fg = rang::fg::green;
    m_bg = rang::bg::reset;
  }

  auto visit(const parse::ExecStmtNode & /*unused*/) -> void override {
    m_fg = rang::fg::blue;
    m_bg = rang::bg::reset;
  }

  auto visit(const parse::FuncDeclStmtNode & /*unused*/) -> void override {
    m_fg = rang::fg::blue;
    m_bg = rang::bg::reset;
  }

  auto visit(const parse::StructDeclStmtNode & /*unused*/) -> void override {
    m_fg = rang::fg::yellow;
    m_bg = rang::bg::reset;
  }

  auto visit(const parse::UnionDeclStmtNode & /*unused*/) -> void override {
    m_fg = rang::fg::yellow;
    m_bg = rang::bg::reset;
  }

private:
  rang::fg m_fg{};
  rang::bg m_bg{};
};

} // namespace parsediag
