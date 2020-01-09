#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/node_visitor_optional.hpp"
#include "parse/type_param_list.hpp"
#include "prog/program.hpp"
#include <optional>

namespace frontend::internal {

class GetIdentifier final : public parse::OptionalNodeVisitor {
public:
  GetIdentifier();

  [[nodiscard]] auto getInstance() const noexcept -> const parse::Node*;
  [[nodiscard]] auto getIdentifier() const noexcept -> const std::optional<lex::Token>&;
  [[nodiscard]] auto getTypeParams() const noexcept -> const std::optional<parse::TypeParamList>&;

  auto visit(const parse::IdExprNode& n) -> void override;

  auto visit(const parse::FieldExprNode& n) -> void override;

private:
  const parse::Node* m_instance;
  std::optional<lex::Token> m_identifier;
  std::optional<parse::TypeParamList> m_typeParams;
};

} // namespace frontend::internal
