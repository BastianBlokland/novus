#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/node_expr_field.hpp"
#include "parse/node_expr_id.hpp"
#include "parse/node_visitor_optional.hpp"
#include "parse/type_param_list.hpp"
#include "prog/program.hpp"
#include <optional>

namespace frontend::internal {

class GetIdentifier final : public parse::OptionalNodeVisitor {
public:
  explicit GetIdentifier(bool includeInstances) :
      m_includeInstances{includeInstances},
      m_instance{nullptr},
      m_isSelf{false},
      m_identifier{std::nullopt},
      m_typeParams{std::nullopt} {}

  [[nodiscard]] auto getInstance() const noexcept -> const parse::Node* { return m_instance; }
  [[nodiscard]] auto isSelf() const noexcept -> bool { return m_isSelf; }
  [[nodiscard]] auto getIdentifier() const noexcept -> const std::optional<lex::Token>& {
    return m_identifier;
  }
  [[nodiscard]] auto getTypeParams() const noexcept -> const std::optional<parse::TypeParamList>& {
    return m_typeParams;
  }

  auto visit(const parse::IdExprNode& n) -> void override {
    m_isSelf     = n.isSelf();
    m_identifier = n.getId();
    m_typeParams = n.getTypeParams();
  }

  auto visit(const parse::FieldExprNode& n) -> void override {
    if (m_includeInstances) {
      m_instance   = &n[0];
      m_identifier = n.getId();
    }
  }

private:
  bool m_includeInstances;
  const parse::Node* m_instance;
  bool m_isSelf;
  std::optional<lex::Token> m_identifier;
  std::optional<parse::TypeParamList> m_typeParams;
};

} // namespace frontend::internal
