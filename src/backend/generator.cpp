#include "backend/generator.hpp"
#include "config.hpp"
#include "internal/gen_expr.hpp"
#include "internal/utilities.hpp"
#include "novasm/assembler.hpp"
#include "prog/sym/const_kind.hpp"
#include "prog/sym/exec_stmt.hpp"
#include "prog/sym/func_decl.hpp"
#include "prog/sym/func_id.hpp"
#include <cassert>
#include <limits>
#include <sstream>

namespace backend {

static auto reserveConsts(novasm::Assembler* asmb, const prog::sym::ConstDeclTable& constTable) {
  // Allocate space for the locals, note: space for inputs is automatically allocated by the vm.
  const auto constCount = constTable.getLocalCount();
  if (constCount > 0) {
    if (constCount > std::numeric_limits<uint16_t>::max()) {
      throw std::logic_error{"More then 65535 constants in one scope are not supported"};
    }
    asmb->addStackAlloc(static_cast<uint16_t>(constCount));
  }
}

static auto
generateFunc(novasm::Assembler* asmb, const prog::Program& program, const prog::sym::FuncDef& func)
    -> std::string {

  auto label = internal::getLabel(program, func.getId());
  asmb->label(label);

  reserveConsts(asmb, func.getConsts());

  // Generate the function body.
  auto genExpr = internal::GenExpr{program, asmb, func.getConsts(), func.getId(), true, 1};
  func.getBody().accept(&genExpr);

  // Root expression always has to produce a single value.
  assert(genExpr.getValuesProduced() == 1);

  // Note: Due to tail calls this return might never be executed.
  asmb->addRet();

  return label;
}

static auto generateExecStmt(
    novasm::Assembler* asmb, const prog::Program& program, const prog::sym::ExecStmt& exec)
    -> std::string {

  auto label = asmb->generateLabel("exec-stmt");
  asmb->label(label);

  reserveConsts(asmb, exec.getConsts());

  // Generate the expression.
  auto genExpr = internal::GenExpr{program, asmb, exec.getConsts(), std::nullopt, false, 1};
  exec.getExpr().accept(&genExpr);

  // TODO(bastian): Support root execute statements not producing values.
  assert(genExpr.getValuesProduced() == 1);

  asmb->addRet();

  return label;
}

auto generate(const prog::Program& program, GenerateFlags flags)
    -> std::pair<novasm::Executable, InstructionLabels> {
  auto compilerVersion = std::string{PROJECT_VER};
  auto asmb            = novasm::Assembler{std::move(compilerVersion)};

  const bool deterministic = (flags & GenerateFlags::Deterministic) != GenerateFlags::None;

  // Generate function definitons.
  internal::forEachFuncDef(program, deterministic, [&](const prog::sym::FuncDef& funcDef) {
    generateFunc(&asmb, program, funcDef);
  });

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
