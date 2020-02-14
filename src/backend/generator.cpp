#include "backend/generator.hpp"
#include "backend/builder.hpp"
#include "internal/gen_expr.hpp"
#include "internal/gen_type_eq.hpp"
#include "internal/utilities.hpp"
#include "prog/sym/const_kind.hpp"
#include "prog/sym/exec_stmt.hpp"
#include "prog/sym/func_decl.hpp"
#include "prog/sym/func_id.hpp"
#include <sstream>

namespace backend {

static auto reserveConsts(Builder* builder, const prog::sym::ConstDeclTable& constTable) {
  // Allocate space for the locals, note: space for inputs is automatically allocated by the vm.
  const auto constCount = constTable.getLocalCount();
  if (constCount > 0) {
    if (constCount > std::numeric_limits<uint8_t>::max()) {
      throw std::logic_error{"More then 256 constants in one scope are not supported"};
    }
    builder->addStackAlloc(static_cast<uint8_t>(constCount));
  }
}

static auto
generateFunc(Builder* builder, const prog::Program& program, const prog::sym::FuncDef& func)
    -> std::string {

  const auto label = internal::getLabel(func.getId());
  builder->label(label);
  reserveConsts(builder, func.getConsts());

  // Generate the function body.
  auto genExpr = internal::GenExpr{program, builder, true};
  func.getExpr().accept(&genExpr);

  // Note: Due to tail calls this return might never be executed.
  builder->addRet();

  return label;
}

static auto
generateExecStmt(Builder* builder, const prog::Program& program, const prog::sym::ExecStmt& exec)
    -> std::string {

  const auto label = builder->generateLabel();
  builder->label(label);

  reserveConsts(builder, exec.getConsts());

  // Generate the expression.
  auto genExpr = internal::GenExpr{program, builder, false};
  exec.getExpr().accept(&genExpr);

  builder->addRet();

  return label;
}

auto generate(const prog::Program& program) -> vm::Assembly {
  auto builder = Builder{};

  // Generate utility functions for user types.
  for (auto tDefItr = program.beginTypeDefs(); tDefItr != program.endTypeDefs(); ++tDefItr) {
    if (std::holds_alternative<prog::sym::StructDef>(tDefItr->second)) {
      const auto& structDef = std::get<prog::sym::StructDef>(tDefItr->second);
      internal::generateStructEquality(&builder, program, structDef);

    } else if (std::holds_alternative<prog::sym::UnionDef>(tDefItr->second)) {
      const auto& unionDef = std::get<prog::sym::UnionDef>(tDefItr->second);
      internal::generateUnionEquality(&builder, program, unionDef);
    }
  }

  // Generate function definitons.
  for (auto funcItr = program.beginFuncDefs(); funcItr != program.endFuncDefs(); ++funcItr) {
    generateFunc(&builder, program, funcItr->second);
  }

  // Generate execution statements.
  std::vector<std::string> execStmtLabels = {};
  for (auto execItr = program.beginExecStmts(); execItr != program.endExecStmts(); ++execItr) {
    auto execLabel = generateExecStmt(&builder, program, *execItr);
    execStmtLabels.push_back(std::move(execLabel));
  }

  if (execStmtLabels.size() == 1) {
    // If there is only a single exec-statement we can simply make that the entry-point.
    builder.setEntrypoint(execStmtLabels[0]);
  } else {
    // If there are multiple exec-statements we create a block of code that calls all of them
    // in-order and make that the entry-point.

    auto entryPointLabel = builder.generateLabel();
    builder.setEntrypoint(entryPointLabel);

    builder.label(entryPointLabel);
    for (const auto& execStmtLabel : execStmtLabels) {
      builder.addCall(execStmtLabel, 0, CallMode::Normal);
    }
    builder.addRet(); // Returning from the root stack-frame causes the program to stop.
  }

  return builder.close();
}

} // namespace backend
