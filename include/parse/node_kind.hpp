#pragma once
#include <iostream>

namespace parse {

enum class NodeKind {
  StmtFuncDecl,
  StmtExec,
  ExprUnaryOp,
  ExprBinaryOp,
  ExprSwitch,
  ExprSwitchIf,
  ExprSwitchElse,
  ExprLit,
  ExprConst,
  ExprConstDecl,
  ExprCall,
  ExprGroup,
  ExprParen,
  Error
};

auto operator<<(std::ostream& out, const NodeKind& rhs) -> std::ostream&;

} // namespace parse
