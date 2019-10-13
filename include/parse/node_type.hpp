#pragma once
#include <iostream>

namespace parse {

enum class NodeType {
  StmtFunDecl,
  StmtPrint,
  ExpUnaryOp,
  ExpBinaryOp,
  ExpSwitch,
  ExpLit,
  ExpConst,
  ExpConstDecl,
  ExpCall,
  ExpComma,
  ExpParan,
  Error
};

auto operator<<(std::ostream& out, const NodeType& rhs) -> std::ostream&;

} // namespace parse
