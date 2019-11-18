#pragma once
#include "frontend/diag.hpp"
#include "frontend/source.hpp"
#include "prog/program.hpp"

namespace frontend::internal {

class TypeInferUserFuncs final {
public:
  TypeInferUserFuncs() = delete;
  TypeInferUserFuncs(const Source& src, prog::Program* prog);

  [[nodiscard]] auto hasErrors() const noexcept -> bool;
  [[nodiscard]] auto getDiags() const noexcept -> const std::vector<Diag>&;

  auto inferTypes(prog::sym::FuncId id, const parse::FuncDeclStmtNode& n) -> void;

private:
  const Source& m_src;
  prog::Program* m_prog;
  std::vector<Diag> m_diags;
};

} // namespace frontend::internal
