#include "prog/copy.hpp"
#include "prog/expr/rewriter.hpp"
#include <stdexcept>

namespace prog {

auto copyType(const Program& from, Program* to, sym::TypeId id) -> bool {
  const auto& fromDecl = from.getTypeDecl(id);
  if (sym::isPrimitive(fromDecl.getKind())) {
    return false;
  }

  auto& toDeclTable = internal::getTypeDeclTable(to);
  if (toDeclTable.exists(id)) {
    return false;
  }

  auto& toDefTable = internal::getTypeDefTable(to);

  // Declare type in the 'to' program.
  toDeclTable.insertType(id, fromDecl.getKind(), fromDecl.getName());

  // Define type in the 'to' program.
  toDefTable.registerType(id, from.getTypeDef(id));
  return true;
}

auto copyFunc(const Program& from, Program* to, sym::FuncId id, prog::expr::Rewriter* rewriter)
    -> bool {

  const auto& fromDecl = from.getFuncDecl(id);

  auto& toDeclTable = internal::getFuncDeclTable(to);
  if (toDeclTable.exists(id)) {
    return false;
  }

  auto& toDefTable = internal::getFuncDefTable(to);

  // Declare function in the 'to' program.
  toDeclTable.insertFunc(
      id,
      fromDecl.getKind(),
      fromDecl.isAction(),
      fromDecl.isImplicitConv(),
      fromDecl.getName(),
      fromDecl.getInput(),
      fromDecl.getOutput());

  // Define function in the 'to' program.
  if (fromDecl.getKind() == sym::FuncKind::User) {
    auto& fromDef = from.getFuncDef(id);
    toDefTable.registerFunc(
        toDeclTable, id, fromDef.getConsts(), fromDef.getExpr().clone(rewriter));
  }
  return true;
}

} // namespace prog
