#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/type.hpp"
#include <vector>

namespace parse {

// Argument list declaration.
// Example in source: '(int i, float f = 42.0)'.
class ArgumentListDecl final {
  friend auto operator<<(std::ostream& out, const ArgumentListDecl& rhs) -> std::ostream&;

public:
  // Argument initializer.
  // Example in source: '= 42'.
  class ArgInitializer final {
  public:
    ArgInitializer(lex::Token eq, NodePtr expr);

    auto operator==(const ArgInitializer& rhs) const noexcept -> bool;

    [[nodiscard]] auto getEq() const noexcept -> const lex::Token&;
    [[nodiscard]] auto getExpr() const noexcept -> const Node&;
    [[nodiscard]] auto takeExpr() noexcept -> NodePtr;

    [[nodiscard]] auto validate() const -> bool;

  private:
    lex::Token m_eq;
    NodePtr m_expr;
  };

  // Argument specification.
  // Example in source: 'int i = 42'.
  class ArgSpec final {
  public:
    ArgSpec(
        Type type, lex::Token identifier, std::optional<ArgInitializer> initializer = std::nullopt);

    auto operator==(const ArgSpec& rhs) const noexcept -> bool;

    [[nodiscard]] auto getType() const noexcept -> const Type&;
    [[nodiscard]] auto getIdentifier() const noexcept -> const lex::Token&;
    [[nodiscard]] auto hasInitializer() const noexcept -> bool;
    [[nodiscard]] auto getInitializer() const noexcept -> const ArgInitializer&;
    [[nodiscard]] auto getInitializer() noexcept -> ArgInitializer&;

    [[nodiscard]] auto validate() const -> bool;

  private:
    Type m_type;
    lex::Token m_identifier;
    std::optional<ArgInitializer> m_initializer;
  };

  using Iterator = typename std::vector<ArgSpec>::const_iterator;

  ArgumentListDecl() = delete;
  ArgumentListDecl(
      lex::Token open, std::vector<ArgSpec> args, std::vector<lex::Token> commas, lex::Token close);

  auto operator==(const ArgumentListDecl& rhs) const noexcept -> bool;
  auto operator!=(const ArgumentListDecl& rhs) const noexcept -> bool;

  [[nodiscard]] auto begin() const -> Iterator;
  [[nodiscard]] auto end() const -> Iterator;
  [[nodiscard]] auto getCount() const -> unsigned int;
  [[nodiscard]] auto getInitializerCount() const -> unsigned int;
  [[nodiscard]] auto getInitializer(unsigned int i) const -> const Node&;
  [[nodiscard]] auto takeInitializer(unsigned int i) -> NodePtr;

  [[nodiscard]] auto getSpan() const -> input::Span;
  [[nodiscard]] auto getOpen() const -> const lex::Token&;
  [[nodiscard]] auto getArgs() const -> const std::vector<ArgSpec>&;
  [[nodiscard]] auto getCommas() const -> const std::vector<lex::Token>&;
  [[nodiscard]] auto getClose() const -> const lex::Token&;

  [[nodiscard]] auto validate() const -> bool;

private:
  lex::Token m_open;
  std::vector<ArgSpec> m_args;
  std::vector<lex::Token> m_commas;
  lex::Token m_close;

  auto print(std::ostream& out) const -> std::ostream&;
};

auto operator<<(std::ostream& out, const ArgumentListDecl& rhs) -> std::ostream&;

} // namespace parse
