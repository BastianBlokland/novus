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

auto copyFunc(const Program& from, Program* to, sym::FuncId id) -> bool {
  auto modified = false;
  return copyFunc(from, to, id, modified, {});
}

auto copyFunc(
    const Program& from,
    Program* to,
    sym::FuncId id,
    bool& modified,
    const RewriterFactory& rewriterFactory) -> bool {

  const auto& fromDecl = from.getFuncDecl(id);

  auto& toDeclTable = internal::getFuncDeclTable(to);
  if (toDeclTable.exists(id)) {
    return false;
  }

  auto& toDefTable = internal::getFuncDefTable(to);

  // NOTE: Optional input initializers are not copied at this time.
  const auto numOptInputs   = 0u;
  auto optInputInitializers = std::vector<expr::NodePtr>{};

  // Declare function in the 'to' program.
  toDeclTable.insertFunc(
      id,
      fromDecl.getKind(),
      fromDecl.isAction(),
      fromDecl.isIntrinsic(),
      fromDecl.isImplicitConv(),
      fromDecl.getName(),
      fromDecl.getInput(),
      fromDecl.getOutput(),
      numOptInputs);

  // Define function in the 'to' program, optionally rewriting the expresion.
  if (fromDecl.getKind() == sym::FuncKind::User) {
    auto& fromDef = from.getFuncDef(id);
    auto consts   = fromDef.getConsts();
    auto rewriter =
        rewriterFactory ? rewriterFactory(from, id, &consts) : std::unique_ptr<expr::Rewriter>{};
    auto newBody =
        rewriter ? rewriter->rewrite(fromDef.getBody()) : fromDef.getBody().clone(nullptr);
    modified = rewriter && rewriter->hasModified();

    toDefTable.registerFunc(
        toDeclTable,
        id,
        std::move(consts),
        std::move(newBody),
        std::move(optInputInitializers),
        fromDef.getFlags());
  }
  return true;
}

} // namespace prog
