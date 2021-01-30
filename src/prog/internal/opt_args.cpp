#include "internal/opt_args.hpp"
#include "prog/expr/node_call.hpp"
#include "prog/expr/node_visitor_deep.hpp"
#include "prog/program.hpp"

namespace prog::internal {

namespace {

class ApplyAllOptArgsVisitor final : public prog::expr::DeepNodeVisitor {
public:
  ApplyAllOptArgsVisitor(const Program& prog) : m_prog{prog} {}

  auto visit(const prog::expr::CallExprNode& n) -> void override;

private:
  const Program& m_prog;
};

auto ApplyAllOptArgsVisitor::visit(const prog::expr::CallExprNode& n) -> void {

  n.applyPatches(m_prog);

  // Also apply optional arguments for any nested calls.
  visitNode(&n);
}

} // namespace

auto applyOptArgIntializers(const Program& prog, sym::FuncId func, std::vector<expr::NodePtr>* args)
    -> void {

  if (!args) {
    throw std::invalid_argument{"Null args pointer provided"};
  }

  const auto& funcDecl = prog.getFuncDecl(func);
  if (args->size() == funcDecl.getInput().getCount()) {
    return; // No need to apply any optional initializers.
  }

  if (funcDecl.getKind() != sym::FuncKind::User) {
    throw std::invalid_argument{"Optional arguments are only supported on user functions"};
  }

  const auto& funcDef = prog.getFuncDef(func);

  const auto numOptInputsToApply = funcDecl.getInput().getCount() - args->size();
  if (numOptInputsToApply > funcDecl.getNumOptInputs()) {
    throw std::invalid_argument{"Too little parameters provided"};
  }

  const auto numOptInputsToSkip = funcDecl.getNumOptInputs() - numOptInputsToApply;
  for (auto i = 0u; i != numOptInputsToApply; ++i) {
    args->push_back(funcDef.getOptArgInitializer(numOptInputsToSkip + i));
  }
}

auto applyOptArgIntializers(const Program& prog) -> void {

  auto visitor = ApplyAllOptArgsVisitor{prog};

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

} // namespace prog::internal
