#include "internal/typeinfer_user_funcs.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"

namespace frontend::internal {

TypeInferUserFuncs::TypeInferUserFuncs(const Source& src, prog::Program* prog) :
    m_src{src}, m_prog{prog} {}

auto TypeInferUserFuncs::hasErrors() const noexcept -> bool { return !m_diags.empty(); }

auto TypeInferUserFuncs::getDiags() const noexcept -> const std::vector<Diag>& { return m_diags; }

auto TypeInferUserFuncs::inferTypes(prog::sym::FuncId id, const parse::FuncDeclStmtNode& n)
    -> void {
  const auto& funcDecl = m_prog->getFuncDecl(id);
  if (!funcDecl.getSig().getOutput().isInferred()) {
    return;
  }

  m_diags.push_back(errUnableToInferFuncReturnType(m_src, funcDecl.getName(), n.getSpan()));
}

} // namespace frontend::internal
