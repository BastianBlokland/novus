#pragma once
#include "frontend/diag.hpp"
#include "frontend/source.hpp"
#include "prog/program.hpp"

namespace frontend::internal {

class TypeInferUserFuncs final {
public:
  TypeInferUserFuncs() = delete;
  explicit TypeInferUserFuncs(prog::Program* prog);

  auto inferRetType(prog::sym::FuncId id, const parse::FuncDeclStmtNode& n) -> bool;

private:
  prog::Program* m_prog;
};

} // namespace frontend::internal
