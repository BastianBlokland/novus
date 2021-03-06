#pragma once
#include "frontend/diag.hpp"
#include "frontend/source_table.hpp"
#include "prog/program.hpp"

namespace frontend::internal {

using PatchDiagReportFunc = std::function<void(Diag)>;

/* Patches all call expressions in the program to include any information that was not available
 * when the call expression was initialally created. One example of this are the optional argument
 * initializers.
 */
auto patchCallArgs(
    const prog::Program& prog, const SourceTable& srcTable, PatchDiagReportFunc reportDiag) -> void;

} // namespace frontend::internal
