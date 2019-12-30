#pragma once
#include "lex/token.hpp"
#include "parse/node_visitor_optional.hpp"
#include "parse/type_param_list.hpp"
#include <optional>

namespace frontend::internal {

class GetIdentifier final : public parse::OptionalNodeVisitor {
public:
  GetIdentifier() = default;

  [[nodiscard]] auto getIdentifier() const noexcept -> const std::optional<lex::Token>&;
  [[nodiscard]] auto getTypeParams() const noexcept -> const std::optional<parse::TypeParamList>&;

  auto visit(const parse::IdExprNode& n) -> void override;

private:
  std::optional<lex::Token> m_identifier;
  std::optional<parse::TypeParamList> m_typeParams;
};

} // namespace frontend::internal
