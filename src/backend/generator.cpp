#include "backend/generator.hpp"
#include "backend/builder.hpp"
#include "internal/gen_expr.hpp"
#include "internal/utilities.hpp"
#include "prog/sym/action_kind.hpp"
#include "prog/sym/const_kind.hpp"
#include "prog/sym/exec_stmt.hpp"
#include "prog/sym/func_decl.hpp"
#include "prog/sym/func_id.hpp"
#include <sstream>

namespace backend {

static auto reserveConsts(Builder* builder, const prog::sym::ConstDeclTable& constTable) {
  const auto constCount = constTable.getCount();
  if (constCount > 0) {
    if (constCount > std::numeric_limits<uint8_t>::max()) {
      throw std::logic_error{"More then 256 constants in one scope are not supported"};
    }
    builder->addReserveConsts(static_cast<uint8_t>(constCount));
  }
}

static auto
generateFunc(Builder* builder, const prog::Program& program, const prog::sym::FuncDef& func) {
  builder->label(internal::getLabel(func.getId()));
  reserveConsts(builder, func.getConsts());

  const auto inputs = func.getConsts().getInputs();
  // Its important to read the constants from right to left as that's how they will be on the stack.
  for (auto itr = inputs.rbegin(); itr != inputs.rend(); ++itr) {
    const auto constId = internal::getConstId(*itr);
    builder->addStoreConst(constId);
  }

  // Generate the function body.
  auto genExpr = internal::GenExpr{program, builder};
  func.getExpr().accept(&genExpr);

  builder->addRet();
  builder->addFail(); // Add a fail between section to aid in detecting invalid programs.
}

static auto
generateExecStmt(Builder* builder, const prog::Program& program, const prog::sym::ExecStmt& exec) {
  const auto label = builder->generateLabel();
  builder->label(label);
  builder->addEntryPoint(label);
  reserveConsts(builder, exec.getConsts());

  // Generate the arguments to the action.
  for (const auto& arg : exec.getArgs()) {
    auto genExpr = internal::GenExpr{program, builder};
    arg->accept(&genExpr);
  }

  const auto& actionDecl = program.getActionDecl(exec.getActionId());
  switch (actionDecl.getKind()) {
  case prog::sym::ActionKind::PrintString:
    builder->addPrintString();
    break;
  }
  builder->addRet();
  builder->addFail(); // Add a fail between section to aid in detecting invalid programs.
}

auto generate(const prog::Program& program) -> vm::Assembly {
  auto builder = Builder{};

  for (auto funcItr = program.beginFuncDefs(); funcItr != program.endFuncDefs(); ++funcItr) {
    generateFunc(&builder, program, funcItr->second);
  }

  for (auto execItr = program.beginExecStmts(); execItr != program.endExecStmts(); ++execItr) {
    generateExecStmt(&builder, program, *execItr);
  }

  return builder.close();
}

} // namespace backend
