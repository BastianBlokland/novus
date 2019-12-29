#pragma once
#include "lex/token.hpp"
#include "parse/node_visitor_optional.hpp"
#include <optional>

namespace frontend::internal {

class GetIdentifier final : public parse::OptionalNodeVisitor {
public:
  GetIdentifier() = default;

  [[nodiscard]] auto getIdentifier() const noexcept -> const std::optional<lex::Token>&;

  auto visit(const parse::IdExprNode& n) -> void override;

private:
  std::optional<lex::Token> m_identifier;
};

} // namespace frontend::internal
