#pragma once
#include "frontend/diag_defs.hpp"
#include "frontend/source.hpp"
#include "lex/token_payload_id.hpp"
#include "parse/node_stmt_func_decl.hpp"
#include "parse/node_visitor_optional.hpp"
#include "prog/program.hpp"
#include <stdexcept>
#include <vector>

namespace frontend::visitors {

inline auto getName(const lex::Token& token) -> std::string {
  if (token.getKind() != lex::TokenKind::Identifier) {
    return "__unknown";
  }
  return token.getPayload<lex::IdentifierTokenPayload>()->getIdentifier();
}

class DeclareUserFuncs final : public parse::OptionalNodeVisitor {
public:
  DeclareUserFuncs() = delete;
  DeclareUserFuncs(const Source& src, prog::Program* prog) : m_src{src}, m_prog{prog}, m_diags{} {
    if (m_prog == nullptr) {
      throw std::invalid_argument{"Program cannot be null"};
    }
  }

  [[nodiscard]] auto hasErrors() const noexcept -> bool { return !m_diags.empty(); }

  [[nodiscard]] auto getDiags() const noexcept -> const std::vector<Diag>& { return m_diags; }

  auto visit(const parse::FuncDeclStmtNode& n) -> void override {
    auto isValid = true;

    // Get func name.
    const auto funcName = getName(n.getId());
    if (m_prog->lookupType(funcName)) {
      m_diags.push_back(errFuncNameConflictsWithType(m_src, funcName, n.getId().getSpan()));
      isValid = false;
    }
    if (!m_prog->lookupActions(funcName).empty()) {
      m_diags.push_back(errFuncNameConflictsWithAction(m_src, funcName, n.getId().getSpan()));
      isValid = false;
    }

    // Get func input.
    auto argTypes = std::vector<prog::sym::TypeId>{};
    for (const auto& arg : n.getArgs()) {
      const auto argTypeName = getName(arg.first);
      const auto argType     = m_prog->lookupType(argTypeName);
      if (argType) {
        argTypes.push_back(argType.value());
      } else {
        m_diags.push_back(errUndeclaredType(m_src, argTypeName, arg.first.getSpan()));
        isValid = false;
      }
    }
    auto input = prog::sym::Input{std::move(argTypes)};

    // Get return type.
    const auto retTypeName = getName(n.getRetType());
    const auto retType     = m_prog->lookupType(retTypeName);
    if (!retType) {
      m_diags.push_back(errUndeclaredType(m_src, retTypeName, n.getRetType().getSpan()));
      isValid = false;
    }

    // Verify that this is not a duplicate declaration.
    if (m_prog->lookupFunc(funcName, input)) {
      m_diags.push_back(errDuplicateFuncDeclaration(m_src, funcName, n.getSpan()));
      isValid = false;
    }

    // Declare the function in the program.
    if (isValid) {
      m_prog->declareUserFunc(funcName, prog::sym::FuncSig{std::move(input), retType.value()});
    }
  }

private:
  const Source& m_src;
  prog::Program* m_prog;
  std::vector<Diag> m_diags;
};

} // namespace frontend::visitors
