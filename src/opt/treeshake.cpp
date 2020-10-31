#include "internal/find_used_funcs.hpp"
#include "internal/find_used_types.hpp"
#include "opt/opt.hpp"
#include "prog/copy.hpp"
#include "prog/sym/func_id_hasher.hpp"
#include "prog/sym/type_id_hasher.hpp"
#include <algorithm>
#include <unordered_set>

namespace opt {

auto treeshake(const prog::Program& prog) -> prog::Program {

  auto funcs = std::unordered_set<prog::sym::FuncId, prog::sym::FuncIdHasher>{};
  auto types = std::unordered_set<prog::sym::TypeId, prog::sym::TypeIdHasher>{};

  // Find all functions that are called from the execute statements.
  auto findFuncs = internal::FindUsedFuncs{prog, &funcs};
  for (auto itr = prog.beginExecStmts(); itr != prog.endExecStmts(); ++itr) {
    itr->getExpr().accept(&findFuncs);
  }

  auto findTypes = internal::FindUsedTypes{prog, &types};

  // Find all types used in the execute statements.
  for (auto itr = prog.beginExecStmts(); itr != prog.endExecStmts(); ++itr) {
    for (const auto& constDecl : itr->getConsts()) {
      findTypes.markType(constDecl.getType());
    }
    itr->getExpr().accept(&findTypes);
  }

  // Find all types used in user functions.
  for (const auto& funcId : funcs) {
    const auto& decl = prog.getFuncDecl(funcId);
    if (decl.getKind() == prog::sym::FuncKind::User) {
      const auto& def = prog.getFuncDef(funcId);

      for (const auto& constDecl : def.getConsts()) {
        findTypes.markType(constDecl.getType());
      }
      def.getExpr().accept(&findTypes);
    }
  }

  // Create a new program and copy the used functions, types and the exec statements.
  auto result = prog::Program{};
  for (const auto& func : funcs) {
    prog::copyFunc(prog, &result, func);
  }
  for (const auto& type : types) {
    prog::copyType(prog, &result, type);
  }
  for (auto execItr = prog.beginExecStmts(); execItr != prog.endExecStmts(); ++execItr) {
    result.addExecStmt(execItr->getConsts(), execItr->getExpr().clone(nullptr));
  }
  return result;
}

} // namespace opt
