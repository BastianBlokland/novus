#include "prog/copy.hpp"
#include <stdexcept>

namespace prog {

auto copyType(const Program& from, Program* to, sym::TypeId id) -> void {
  const auto& fromDecl = from.getTypeDecl(id);

  auto& toDeclTable = internal::getTypeDeclTable(to);
  auto& toDefTable  = internal::getTypeDefTable(to);

  // Declare type in the 'to' program.
  toDeclTable.insertType(id, fromDecl.getKind(), fromDecl.getName());

  // Define type in the 'to' program.
  toDefTable.registerType(id, from.getTypeDef(id));
}

auto copyUserFunc(const Program& from, Program* to, sym::FuncId id) -> void {
  const auto& fromDecl = from.getFuncDecl(id);
  if (fromDecl.getKind() != sym::FuncKind::User) {
    throw std::invalid_argument{"Function is not a user-declared function"};
  }

  auto& toDeclTable = internal::getFuncDeclTable(to);
  auto& toDefTable  = internal::getFuncDefTable(to);

  // Declare function in the 'to' program.
  toDeclTable.insertFunc(
      id,
      fromDecl.getKind(),
      fromDecl.isAction(),
      fromDecl.isImplicitConv(),
      fromDecl.getName(),
      fromDecl.getInput(),
      fromDecl.getOutput());

  auto& fromDef = from.getFuncDef(id);

  // Define function in the 'to' program.
  toDefTable.registerFunc(toDeclTable, id, fromDef.getConsts(), fromDef.getExpr().clone());
}

} // namespace prog
