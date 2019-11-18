#include "internal/typeinfer_user_funcs.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/typeinfer_expr.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"

namespace frontend::internal {

TypeInferUserFuncs::TypeInferUserFuncs(const Source& src, prog::Program* prog) :
    m_src{src}, m_prog{prog} {}

auto TypeInferUserFuncs::hasErrors() const noexcept -> bool { return !m_diags.empty(); }

auto TypeInferUserFuncs::getDiags() const noexcept -> const std::vector<Diag>& { return m_diags; }

auto TypeInferUserFuncs::inferTypes(prog::sym::FuncId id, const parse::FuncDeclStmtNode& n)
    -> void {
  auto& funcDecl = m_prog->getFuncDecl(id);
  if (!funcDecl.getSig().getOutput().isInferred()) {
    return;
  }

  auto constTypes = std::unordered_map<std::string, prog::sym::TypeId>{};
  for (const auto& arg : n.getArgs()) {
    const auto argType = m_prog->lookupType(getName(arg.getType()));
    if (argType) {
      constTypes.insert({getName(arg.getIdentifier()), *argType});
    }
  }

  auto inferBodyType = TypeInferExpr{m_prog, &constTypes};
  n[0].accept(&inferBodyType);
  const auto type = inferBodyType.getType();

  // If type is still not a concrete type then we fail.
  if (type.isInferred()) {
    m_diags.push_back(errUnableToInferFuncReturnType(m_src, funcDecl.getName(), n.getSpan()));
    return;
  }

  // Update signature with inferred type.
  m_prog->updateFuncRetType(id, type);
}

} // namespace frontend::internal
