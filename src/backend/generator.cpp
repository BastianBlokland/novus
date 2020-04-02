#include "backend/generator.hpp"
#include "internal/gen_expr.hpp"
#include "internal/gen_type_eq.hpp"
#include "internal/utilities.hpp"
#include "novasm/assembler.hpp"
#include "prog/sym/const_kind.hpp"
#include "prog/sym/exec_stmt.hpp"
#include "prog/sym/func_decl.hpp"
#include "prog/sym/func_id.hpp"
#include <sstream>

namespace backend {

static auto reserveConsts(novasm::Assembler* asmb, const prog::sym::ConstDeclTable& constTable) {
  // Allocate space for the locals, note: space for inputs is automatically allocated by the vm.
  const auto constCount = constTable.getLocalCount();
  if (constCount > 0) {
    if (constCount > std::numeric_limits<uint8_t>::max()) {
      throw std::logic_error{"More then 256 constants in one scope are not supported"};
    }
    asmb->addStackAlloc(static_cast<uint8_t>(constCount));
  }
}

static auto
generateFunc(novasm::Assembler* asmb, const prog::Program& program, const prog::sym::FuncDef& func)
    -> std::string {

  const auto label = internal::getLabel(program, func.getId());
  asmb->label(label);

  reserveConsts(asmb, func.getConsts());

  // Generate the function body.
  auto genExpr = internal::GenExpr{program, asmb, func.getConsts(), func.getId(), true};
  func.getExpr().accept(&genExpr);

  // Note: Due to tail calls this return might never be executed.
  asmb->addRet();

  return label;
}

static auto generateExecStmt(
    novasm::Assembler* asmb, const prog::Program& program, const prog::sym::ExecStmt& exec)
    -> std::string {

  const auto label = asmb->generateLabel("exec-stmt");
  asmb->label(label);

  reserveConsts(asmb, exec.getConsts());

  // Generate the expression.
  auto genExpr = internal::GenExpr{program, asmb, exec.getConsts(), std::nullopt, false};
  exec.getExpr().accept(&genExpr);

  asmb->addRet();

  return label;
}

auto generate(const prog::Program& program) -> std::pair<novasm::Assembly, InstructionLabels> {
  auto asmb = novasm::Assembler{};

  // Generate equality functions for user types (structs and unions).
  internal::generateUserTypeEquality(&asmb, program);

  // Generate function definitons.
  for (auto funcItr = program.beginFuncDefs(); funcItr != program.endFuncDefs(); ++funcItr) {
    const auto& funcDef = program.getFuncDef(*funcItr);
    generateFunc(&asmb, program, funcDef);
  }

  // Generate execution statements.
  std::vector<std::string> execStmtLabels = {};
  for (auto execItr = program.beginExecStmts(); execItr != program.endExecStmts(); ++execItr) {
    auto execLabel = generateExecStmt(&asmb, program, *execItr);
    execStmtLabels.push_back(std::move(execLabel));
  }

  if (execStmtLabels.size() == 1) {
    // If there is only a single exec-statement we can simply make that the entry-point.
    asmb.setEntrypoint(execStmtLabels[0]);
  } else {
    // If there are multiple exec-statements we create a block of code that calls all of them
    // in-order and make that the entry-point.

    auto entryPointLabel = asmb.generateLabel("entrypoint");
    asmb.setEntrypoint(entryPointLabel);

    asmb.label(entryPointLabel);
    for (const auto& execStmtLabel : execStmtLabels) {
      asmb.addCall(execStmtLabel, 0, novasm::CallMode::Normal);
    }
    asmb.addRet(); // Returning from the root stack-frame causes the program to stop.
  }

  return std::make_pair(asmb.close(), asmb.getLabels());
}

} // namespace backend
