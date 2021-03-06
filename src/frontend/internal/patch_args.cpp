#include "internal/patch_args.hpp"
#include "frontend/diag_defs.hpp"
#include "prog/expr/node_call.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_lit_string.hpp"
#include "prog/expr/node_visitor_deep.hpp"
#include <cassert>
#include <optional>

namespace frontend::internal {

namespace {

/* Maximum depth we allow while recursivly applying optional arguments. If we recurse deeper then
 * this we consider it an 'cycle' and fail the compilation.
 * TODO: Investigate if there are smarter ways to detect this, its not very trivial as you can have
 * indirect recursion in your initializers.
 */
constexpr size_t g_maxOptArgRecurseDepth = 100;

/* Replace calls to the source-location intrinsics with literals containing the source info.
 */
auto patchSourceLocIntrinsic(
    const prog::Program& prog,
    const SourceTable& srcTable,
    prog::sym::SourceId srcId,
    prog::expr::NodePtr& expr) {

  if (expr->getKind() != prog::expr::NodeKind::Call) {
    return;
  }

  const auto& call         = *expr->downcast<prog::expr::CallExprNode>();
  const auto& callFuncDecl = prog.getFuncDecl(call.getFunc());
  switch (callFuncDecl.getKind()) {
  case prog::sym::FuncKind::SourceLocFile:
    expr = prog::expr::litStringNode(prog, srcTable[srcId].getId());
    break;
  case prog::sym::FuncKind::SourceLocLine:
    expr = prog::expr::litIntNode(prog, srcTable[srcId].getStart().getLine());
    break;
  case prog::sym::FuncKind::SourceLocColumn:
    expr = prog::expr::litIntNode(prog, srcTable[srcId].getStart().getCol());
    break;
  default:
    break;
  }
}

/* Node visitor that patches all calls that did not supply overrides for optional arguments by
 * copying the optional arg initializers to the callsite.
 */
class PatchOptArgsVisitor final : public prog::expr::DeepNodeVisitor {
public:
  PatchOptArgsVisitor(
      const prog::Program& prog, const SourceTable& srcTable, PatchDiagReportFunc reportFunc) :
      m_prog{prog},
      m_srcTable{srcTable},
      m_reportFunc{std::move(reportFunc)},
      m_optArgRecurseDepth{0},
      m_rootSrcId{std::nullopt} {}

  auto visit(const prog::expr::CallExprNode& n) -> void override;

private:
  const prog::Program& m_prog;
  const SourceTable& m_srcTable;
  PatchDiagReportFunc m_reportFunc;

  size_t m_optArgRecurseDepth;

  // Root of this call to be used by SourceLoc intrinsics.
  std::optional<prog::sym::SourceId> m_rootSrcId;
};

auto PatchOptArgsVisitor::visit(const prog::expr::CallExprNode& n) -> void {

  bool hadOptArgs = false;

  // Apply any optional argument initializers.
  n.patchArgs([this, &hadOptArgs](prog::sym::FuncId func, std::vector<prog::expr::NodePtr>* args) {
    const auto& funcDecl = this->m_prog.getFuncDecl(func);
    if (args->size() == funcDecl.getInput().getCount()) {
      return; // No need to apply any optional initializers.
    }
    assert(funcDecl.getKind() == prog::sym::FuncKind::User);

    const auto& funcDef = this->m_prog.getFuncDef(func);

    const auto numOptInputsToApply = funcDecl.getInput().getCount() - args->size();
    if (numOptInputsToApply > funcDecl.getNumOptInputs()) {
      throw std::invalid_argument{"Too little parameters provided"};
    }

    const auto numOptInputsToSkip = funcDecl.getNumOptInputs() - numOptInputsToApply;
    for (auto i = 0u; i != numOptInputsToApply; ++i) {
      args->push_back(funcDef.getOptArgInitializer(numOptInputsToSkip + i));
      hadOptArgs = true;
    }
  });

  // This expression becomes a root for 'SourceLocation' intrinsics if its the top-most call that
  // applies any optional arguments. Reason for this is that 'SourceLocation' intrinsics are to be
  // used inside optional argument initializers.
  const bool isSrcIdRoot = hadOptArgs && n.hasSourceId() && !m_rootSrcId.has_value();
  if (isSrcIdRoot) {
    m_rootSrcId = n.getSourceId();
  }

  if (hadOptArgs) {
    if (++m_optArgRecurseDepth > g_maxOptArgRecurseDepth) {
      // TODO: How should we handle getting here without a source-id? Which is theorectically
      // possible if we didn't associate this expression with any source.
      assert(m_rootSrcId);
      m_reportFunc(errCyclicOptArgInitializer(*m_rootSrcId));
      return;
    }
  }

  // Recurse to apply optional arguments for any nested calls.
  visitNode(&n);

  // Patch any source-location intrinsics in the call arguments.
  // NOTE: Its important to do this after recursing so that nested source-locations will be
  // attributed to the root call.
  if (m_rootSrcId) {
    n.patchArgs([this](prog::sym::FuncId /*unused*/, std::vector<prog::expr::NodePtr>* args) {
      for (auto& arg : *args) {
        patchSourceLocIntrinsic(this->m_prog, this->m_srcTable, *this->m_rootSrcId, arg);
      }
    });
  }

  if (isSrcIdRoot) {
    m_rootSrcId = std::nullopt;
  }

  if (hadOptArgs) {
    --m_optArgRecurseDepth;
  }
}

} // namespace

auto patchCallArgs(
    const prog::Program& prog, const SourceTable& srcTable, PatchDiagReportFunc reportFunc)
    -> void {

  auto visitor = PatchOptArgsVisitor{prog, srcTable, reportFunc};

  for (auto itr = prog.beginFuncDefs(); itr != prog.endFuncDefs(); ++itr) {
    auto funcId         = *itr;
    const auto& funcDef = prog.getFuncDef(funcId);
    funcDef.getBody().accept(&visitor);
  }

  for (auto itr = prog.beginExecStmts(); itr != prog.endExecStmts(); ++itr) {
    const auto& execStmt = *itr;
    execStmt.getExpr().accept(&visitor);
  }
}

} // namespace frontend::internal
