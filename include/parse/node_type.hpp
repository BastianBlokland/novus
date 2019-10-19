#pragma once
#include <iostream>

namespace parse {

enum class NodeType {
  StmtFunDecl,
  StmtPrint,
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

auto operator<<(std::ostream& out, const NodeType& rhs) -> std::ostream&;

} // namespace parse
