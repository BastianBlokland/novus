#pragma once
#include <iostream>

namespace parse {

enum class NodeType {
  StmtFunDecl,
  StmtPrint,
  ExprUnaryOp,
  ExprBinaryOp,
  ExprSwitch,
  ExprLit,
  ExprConst,
  ExprConstDecl,
  ExprCall,
  ExprComma,
  ExprParen,
  Error
};

auto operator<<(std::ostream& out, const NodeType& rhs) -> std::ostream&;

} // namespace parse
