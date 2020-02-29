#pragma once
#include "prog/expr/node_visitor_deep.hpp"
#include "prog/program.hpp"
#include "prog/sym/func_id_hasher.hpp"
#include <unordered_set>

namespace opt::internal {

class FindFuncs final : public prog::expr::DeepNodeVisitor {

  using FuncSet = typename std::unordered_set<prog::sym::FuncId, prog::sym::FuncIdHasher>;

public:
  FindFuncs(const prog::Program& prog, FuncSet* funcs);

  auto visit(const prog::expr::CallExprNode& n) -> void override;
  auto visit(const prog::expr::ClosureNode& n) -> void override;
  auto visit(const prog::expr::LitFuncNode& n) -> void override;

private:
  const prog::Program& m_prog;
  FuncSet* m_funcs;

  auto markFunc(prog::sym::FuncId func) -> void;
};

} // namespace opt::internal
